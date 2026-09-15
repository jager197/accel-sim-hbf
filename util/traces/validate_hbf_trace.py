#!/usr/bin/env python3
"""Validate HBF lifecycle traces and conservation invariants."""

from __future__ import annotations

import argparse
import csv
import json
import sys
from collections import defaultdict
from pathlib import Path
from typing import Iterable

SCHEMA_VERSION = "hbf-trace-v1"
SCHEMA = [
    "sim_cycle",
    "request_id",
    "source_subpartition",
    "op",
    "address",
    "page",
    "channel",
    "subarray",
    "state",
    "bytes",
    "queue_depth",
    "latency",
    "cache_hit",
    "mshr_hit",
    "error",
]


def as_int(row: dict[str, str], key: str, line: int) -> int:
    try:
        return int(row[key], 0)
    except (KeyError, TypeError, ValueError) as exc:
        raise ValueError(f"line {line}: {key} is not an integer") from exc


def validate(
    path: Path,
    channels: int | None,
    subarrays: int | None,
    expected: int | None,
    require_stable_subarray: bool = False,
    require_nonempty: bool = False,
) -> dict:
    errors: list[str] = []
    requests: dict[str, list[tuple[int, dict[str, str]]]] = defaultdict(list)
    page_channels: dict[int, set[int]] = defaultdict(set)
    page_subarrays: dict[int, set[int]] = defaultdict(set)
    state_counts: dict[str, int] = defaultdict(int)
    row_count = 0
    with path.open(newline="") as stream:
        reader = csv.DictReader(stream)
        fields = reader.fieldnames or []
        schema_exact = fields == SCHEMA
        if not schema_exact:
            errors.append(
                "schema mismatch: expected ordered columns "
                + ",".join(SCHEMA)
                + "; observed "
                + ",".join(fields)
            )
        for line, row in enumerate(reader, start=2):
            row_count += 1
            request_id = row.get("request_id", "")
            if request_id == "":
                errors.append(f"line {line}: empty request_id")
                continue
            try:
                cycle = as_int(row, "sim_cycle", line)
                page = as_int(row, "page", line)
                channel = as_int(row, "channel", line)
                subarray = as_int(row, "subarray", line)
                queue_depth = as_int(row, "queue_depth", line)
                latency = as_int(row, "latency", line)
                bytes_value = as_int(row, "bytes", line)
            except ValueError as exc:
                errors.append(str(exc))
                continue
            if cycle < 0 or page < 0 or queue_depth < 0 or latency < 0 or bytes_value < 0:
                errors.append(f"line {line}: negative numeric field")
            if channels is not None and not 0 <= channel < channels:
                errors.append(f"line {line}: channel {channel} outside [0,{channels})")
            if subarrays is not None and not 0 <= subarray < subarrays:
                errors.append(f"line {line}: subarray {subarray} outside [0,{subarrays})")
            state = row.get("state", "")
            state_counts[state] += 1
            requests[request_id].append((line, row))
            page_channels[page].add(channel)
            if state not in {"INGRESS", ""} and subarray >= 0:
                page_subarrays[page].add(subarray)

    for page, owners in page_channels.items():
        if len(owners) != 1:
            errors.append(f"page {page} maps to multiple channels: {sorted(owners)}")
    if require_stable_subarray:
        for page, owners in page_subarrays.items():
            if len(owners) != 1:
                errors.append(
                    f"page {page} maps to multiple subarrays: {sorted(owners)}"
                )

    ingress = 0
    completed = 0
    for request_id, events in requests.items():
        states = [row.get("state", "") for _, row in events]
        ingress_events = [event for event in events if event[1].get("state") == "INGRESS"]
        completed_events = [event for event in events if event[1].get("state") == "COMPLETED"]
        ingress += len(ingress_events)
        completed += len(completed_events)
        if len(ingress_events) != 1:
            errors.append(f"request {request_id}: expected one INGRESS, got {len(ingress_events)}")
        if len(completed_events) != 1:
            errors.append(f"request {request_id}: expected one COMPLETED, got {len(completed_events)}")
        cycles = [as_int(row, "sim_cycle", line) for line, row in events if row.get("sim_cycle", "") != ""]
        if cycles != sorted(cycles):
            errors.append(f"request {request_id}: event cycles are not monotonic")
        if ingress_events and completed_events:
            in_row = ingress_events[0][1]
            out_row = completed_events[0][1]
            if in_row.get("op") != out_row.get("op"):
                errors.append(f"request {request_id}: op changed from ingress to completion")
            if in_row.get("address") != out_row.get("address"):
                errors.append(f"request {request_id}: address changed from ingress to completion")

    if require_nonempty and (row_count == 0 or not requests):
        errors.append("trace contains no HBF lifecycle traffic")
    if expected is not None and len(requests) != expected:
        errors.append(f"expected {expected} request ids, observed {len(requests)}")
    summary = {
        "schema_version": SCHEMA_VERSION,
        "schema_exact": schema_exact,
        "columns": fields,
        "trace": str(path.resolve()),
        "rows": row_count,
        "request_ids": len(requests),
        "ingress_events": ingress,
        "completion_events": completed,
        "page_count": len(page_channels),
        "state_counts": dict(sorted(state_counts.items())),
        "channel_count_observed": len({channel for values in page_channels.values() for channel in values}),
        "stable_subarray_required": require_stable_subarray,
        "nonempty_required": require_nonempty,
        "valid": not errors,
        "errors": errors,
    }
    return summary


def main(argv: Iterable[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("trace", type=Path)
    parser.add_argument("--channels", type=int)
    parser.add_argument("--subarrays", type=int)
    parser.add_argument("--expected-requests", type=int)
    parser.add_argument(
        "--require-stable-subarray",
        action="store_true",
        help="require one physical subarray per page (disable for FTL remapping traces)",
    )
    parser.add_argument(
        "--require-nonempty",
        action="store_true",
        help="reject header-only traces and traces without request IDs",
    )
    parser.add_argument("--json", type=Path, help="write the validation summary here")
    args = parser.parse_args(argv)
    if args.channels is not None and args.channels <= 0:
        parser.error("--channels must be positive")
    if args.subarrays is not None and args.subarrays <= 0:
        parser.error("--subarrays must be positive")
    if args.expected_requests is not None and args.expected_requests < 0:
        parser.error("--expected-requests must be non-negative")
    summary = validate(
        args.trace,
        args.channels,
        args.subarrays,
        args.expected_requests,
        args.require_stable_subarray,
        args.require_nonempty,
    )
    encoded = json.dumps(summary, indent=2, sort_keys=True)
    if args.json:
        args.json.parent.mkdir(parents=True, exist_ok=True)
        args.json.write_text(encoded + "\n")
    print(encoded)
    return 0 if summary["valid"] else 1


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except OSError as exc:
        print(f"validate_hbf_trace.py: {exc}", file=sys.stderr)
        raise SystemExit(2)
