#!/usr/bin/env python3
"""Replay HBF request traces without a GPU front end.

The replay is intentionally deterministic.  It consumes the same ingress CSV
schema emitted by the simulator, approximates page-level MSHR/write-buffer grouping,
channel and subarray affinity, NAND timing, and return-link serialization, and
emits the same lifecycle schema. Its serialized channel-service abstraction
and lack of a page cache differ from the C++ controller. It is a diagnostic
reference, not an independent validation oracle or a guaranteed upper bound.
"""

from __future__ import annotations

import argparse
import csv
import heapq
import json
import math
import sys
from collections import defaultdict, deque
from dataclasses import dataclass, field
from pathlib import Path
from typing import Iterable

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


def parse_int(value: str) -> int:
    return int(value.strip(), 0)


@dataclass(frozen=True)
class Request:
    request_id: str
    source_subpartition: int
    op: str
    address: int
    bytes: int
    arrival: int


@dataclass
class PageState:
    token: int
    page: int
    channel: int
    subarray: int
    op: str
    first_arrival: int
    request_ids: list[str] = field(default_factory=list)
    request_bytes: int = 0
    written_offsets: set[int] = field(default_factory=set)
    ready: bool = False
    issued: bool = False
    error: str = ""


@dataclass(order=True)
class Completion:
    finish: int
    sequence: int
    token: int = field(compare=False)


def read_requests(path: Path) -> list[Request]:
    requests: list[Request] = []
    with path.open(newline="") as stream:
        reader = csv.DictReader(stream)
        if not reader.fieldnames or "sim_cycle" not in reader.fieldnames:
            raise ValueError("input CSV must contain sim_cycle")
        has_state = "state" in reader.fieldnames
        for line, row in enumerate(reader, start=2):
            if has_state and row.get("state", "") not in {"", "INGRESS"}:
                continue
            try:
                request_id = row["request_id"]
                op = row.get("op", "R").strip().upper()
                if op not in {"R", "W"}:
                    raise ValueError(f"unsupported op {op}")
                requests.append(
                    Request(
                        request_id=request_id,
                        source_subpartition=parse_int(row.get("source_subpartition", "0")),
                        op=op,
                        address=parse_int(row["address"]),
                        bytes=parse_int(row.get("bytes", "64")),
                        arrival=parse_int(row["sim_cycle"]),
                    )
                )
            except (KeyError, ValueError) as exc:
                raise ValueError(f"line {line}: malformed request ({exc})") from exc
    requests.sort(key=lambda request: (request.arrival, request.request_id))
    seen: set[str] = set()
    for request in requests:
        if request.request_id in seen:
            raise ValueError(f"duplicate request_id {request.request_id}")
        if request.bytes <= 0 or request.arrival < 0:
            raise ValueError(f"invalid request {request.request_id}")
        seen.add(request.request_id)
    return requests


class DeviceReplay:
    def __init__(self, args: argparse.Namespace):
        self.base = args.base
        self.span = args.span
        self.page_size = args.page_size
        self.channels = max(1, args.channels)
        self.subarrays = max(self.channels, args.subarrays)
        self.subarrays_per_channel = max(1, self.subarrays // self.channels)
        self.max_active = max(1, args.max_active)
        self.t_read = max(1, args.t_read)
        self.t_prog = max(1, args.t_prog)
        self.t_erase = max(0, args.t_erase)
        self.read_mode = args.read_mode
        self.read_window = max(0, args.read_agg_window)
        self.read_threshold = max(1, args.read_agg_threshold)
        self.write_timeout = max(0, args.write_timeout)
        self.write_policy = args.write_timeout_policy
        self.bytes_per_tick = max(
            1e-9, args.channel_bw_gbps * 1000.0 / max(1e-9, args.dram_freq_mhz)
        )

        self.page_queues: dict[int, deque[PageState]] = defaultdict(deque)
        self.states: dict[int, PageState] = {}
        self.waiting: deque[int] = deque()
        self.release_events: list[tuple[int, int, int, str]] = []
        self.completions: list[Completion] = []
        self.channel_busy_until = [0] * self.channels
        self.subarray_busy_until = [0] * self.subarrays
        self.next_token = 1
        self.next_sequence = 1
        self.next_release_sequence = 1
        self.programmed_pages: set[int] = set()
        self.rows: list[dict[str, object]] = []
        self.stats: dict[str, int | float] = {
            "requests": 0,
            "mshr_hits": 0,
            "page_reads": 0,
            "page_programs": 0,
            "block_erases": 0,
            "requested_read_bytes": 0,
            "media_read_bytes": 0,
            "bytes_read": 0,
            "bytes_written": 0,
            "aggregation_wait": 0,
            "aggregation_pages": 0,
            "first_request_latency": 0,
            "first_request_pages": 0,
            "partial_page_programs": 0,
            "incomplete_page_errors": 0,
            "channel_busy_ticks": [0] * self.channels,
            "max_queue_depth": 0,
        }

    def page_and_channel(self, address: int) -> tuple[int, int, int]:
        if address < self.base or address >= self.base + self.span:
            raise ValueError(f"address 0x{address:x} is outside the HBF span")
        page = (address - self.base) // self.page_size
        channel = page % self.channels
        local = page // self.channels
        subarray = channel * self.subarrays_per_channel + (local % self.subarrays_per_channel)
        if subarray >= self.subarrays:
            subarray = self.subarrays - 1
        return page, channel, subarray

    def add_row(
        self,
        cycle: int,
        request: Request,
        state: str,
        page: int,
        channel: int,
        subarray: int,
        bytes_value: int,
        latency: int = 0,
        cache_hit: int = 0,
        mshr_hit: int = 0,
        error: str = "",
    ) -> None:
        self.rows.append(
            {
                "sim_cycle": cycle,
                "request_id": request.request_id,
                "source_subpartition": request.source_subpartition,
                "op": request.op,
                "address": request.address,
                "page": page,
                "channel": channel,
                "subarray": subarray,
                "state": state,
                "bytes": bytes_value,
                "queue_depth": len(self.states) + len(self.completions),
                "latency": latency,
                "cache_hit": cache_hit,
                "mshr_hit": mshr_hit,
                "error": error,
            }
        )

    def queue_timer(self, deadline: int, token: int, kind: str) -> None:
        heapq.heappush(
            self.release_events,
            (deadline, self.next_release_sequence, token, kind),
        )
        self.next_release_sequence += 1

    def add_write_coverage(self, state: PageState, request: Request) -> None:
        offset = (request.address - self.base) % self.page_size
        if offset + request.bytes > self.page_size:
            raise ValueError("write request crosses a page boundary")
        state.written_offsets.update(range(offset, offset + request.bytes))

    def enqueue_state(self, request: Request, page: int, channel: int, subarray: int) -> PageState:
        state = PageState(
            token=self.next_token,
            page=page,
            channel=channel,
            subarray=subarray,
            op=request.op,
            first_arrival=request.arrival,
            request_ids=[request.request_id],
            request_bytes=request.bytes,
        )
        if request.op == "W":
            self.add_write_coverage(state, request)
        self.next_token += 1
        self.states[state.token] = state
        self.page_queues[page].append(state)
        self.waiting.append(state.token)
        if state.op == "R":
            if self.read_mode == "demand" or request.bytes >= self.read_threshold:
                state.ready = True
            else:
                self.queue_timer(state.first_arrival + self.read_window, state.token, "read")
        else:
            if len(state.written_offsets) >= self.page_size:
                state.ready = True
            else:
                self.queue_timer(state.first_arrival + self.write_timeout, state.token, "write")
        return state

    def ingest(self, request: Request, cycle: int) -> None:
        page, channel, subarray = self.page_and_channel(request.address)
        self.stats["requests"] = int(self.stats["requests"]) + 1
        if request.op == "R":
            self.stats["requested_read_bytes"] = int(self.stats["requested_read_bytes"]) + request.bytes
        self.add_row(cycle, request, "INGRESS", page, channel, 0, request.bytes)
        queue = self.page_queues[page]
        tail = queue[-1] if queue else None
        if tail is not None and tail.op == request.op and (request.op == "R" or not tail.issued):
            tail.request_ids.append(request.request_id)
            tail.request_bytes += request.bytes
            if request.op == "W":
                self.add_write_coverage(tail, request)
            self.stats["mshr_hits"] = int(self.stats["mshr_hits"]) + 1
            if request.op == "R" and tail.ready is False and tail.request_bytes >= self.read_threshold:
                tail.ready = True
            if request.op == "W" and len(tail.written_offsets) >= self.page_size:
                tail.ready = True
            return
        if tail is not None and tail.op == "W" and request.op == "R" and not tail.issued:
            tail.ready = True
        self.enqueue_state(request, page, channel, subarray)

    def release_due(self, cycle: int) -> None:
        while self.release_events and self.release_events[0][0] <= cycle:
            deadline, _, token, kind = heapq.heappop(self.release_events)
            state = self.states.get(token)
            if state is None or state.issued or state.ready:
                continue
            state.ready = True
            if kind == "read":
                self.stats["aggregation_pages"] = int(self.stats["aggregation_pages"]) + 1
                self.stats["aggregation_wait"] = int(self.stats["aggregation_wait"]) + cycle - state.first_arrival
            elif len(state.written_offsets) < self.page_size:
                if self.write_policy == "strict":
                    state.error = "incomplete_page_policy"
                    self.stats["incomplete_page_errors"] = int(self.stats["incomplete_page_errors"]) + 1

    def choose_state(self, cycle: int) -> PageState | None:
        candidate: PageState | None = None
        retained: deque[int] = deque()
        while self.waiting:
            token = self.waiting.popleft()
            state = self.states.get(token)
            if state is None or state.issued:
                continue
            queue = self.page_queues.get(state.page)
            if queue is None or not queue or queue[0].token != token:
                retained.append(token)
                continue
            if not state.ready:
                retained.append(token)
                continue
            if self.channel_busy_until[state.channel] > cycle:
                retained.append(token)
                continue
            if self.subarray_busy_until[state.subarray] > cycle:
                retained.append(token)
                continue
            candidate = state
            break
        self.waiting.extendleft(reversed(retained))
        return candidate

    def issue_ready(self, cycle: int) -> None:
        while len(self.completions) < self.max_active:
            state = self.choose_state(cycle)
            if state is None:
                return
            first_request = next(
                request for request in self.current_requests if request.request_id == state.request_ids[0]
            )
            if (state.op == "W" and self.write_policy == "strict"
                    and len(state.written_offsets) < self.page_size and not state.error):
                state.error = "incomplete_page_policy"
                self.stats["incomplete_page_errors"] = int(self.stats["incomplete_page_errors"]) + 1
            if state.error:
                # Strict mode rejects an incomplete page instead of reporting a
                # short write as a successful full-page program.
                state.issued = True
                heapq.heappush(self.completions, Completion(cycle, self.next_sequence, state.token))
                self.next_sequence += 1
                continue
            duration = self.t_read if state.op == "R" else self.t_prog
            if state.op == "W":
                if state.page in self.programmed_pages:
                    self.stats["block_erases"] = int(self.stats["block_erases"]) + 1
                    duration += self.t_erase
                    self.add_row(cycle, first_request, "ERASE", state.page, state.channel, state.subarray, 0)
                if len(state.written_offsets) < self.page_size:
                    self.stats["partial_page_programs"] = int(self.stats["partial_page_programs"]) + 1
                self.stats["page_programs"] = int(self.stats["page_programs"]) + 1
                self.add_row(cycle, first_request, "PROGRAM", state.page, state.channel, state.subarray, state.request_bytes)
                self.programmed_pages.add(state.page)
            else:
                self.stats["page_reads"] = int(self.stats["page_reads"]) + 1
                self.stats["media_read_bytes"] = int(self.stats["media_read_bytes"]) + self.page_size
                self.add_row(cycle, first_request, "READ", state.page, state.channel, state.subarray, self.page_size)
            transfer_ticks = 0
            if state.op == "R":
                transfer_ticks = math.ceil(state.request_bytes / self.bytes_per_tick)
            finish = cycle + duration + transfer_ticks
            state.issued = True
            self.channel_busy_until[state.channel] = finish
            self.subarray_busy_until[state.subarray] = finish
            self.stats["channel_busy_ticks"][state.channel] += duration
            heapq.heappush(self.completions, Completion(finish, self.next_sequence, state.token))
            self.next_sequence += 1

    def complete_due(self, cycle: int) -> None:
        while self.completions and self.completions[0].finish <= cycle:
            completion = heapq.heappop(self.completions)
            state = self.states.get(completion.token)
            if state is None:
                continue
            error = state.error
            for request in self.current_requests:
                if request.request_id not in state.request_ids:
                    continue
                latency = cycle - request.arrival
                self.add_row(
                    cycle,
                    request,
                    "COMPLETED",
                    state.page,
                    state.channel,
                    state.subarray,
                    request.bytes,
                    latency,
                    0,
                    0,
                    error,
                )
                if request.op == "R":
                    self.stats["bytes_read"] = int(self.stats["bytes_read"]) + request.bytes
                else:
                    self.stats["bytes_written"] = int(self.stats["bytes_written"]) + request.bytes
                if request.request_id == state.request_ids[0]:
                    self.stats["first_request_latency"] = int(self.stats["first_request_latency"]) + latency
                    self.stats["first_request_pages"] = int(self.stats["first_request_pages"]) + 1
            queue = self.page_queues[state.page]
            if queue and queue[0].token == state.token:
                queue.popleft()
            if not queue:
                self.page_queues.pop(state.page, None)
            self.states.pop(state.token, None)

    def run(self, requests: list[Request]) -> tuple[list[dict[str, object]], dict]:
        self.current_requests = requests
        index = 0
        cycle = 0
        while index < len(requests) or self.states or self.completions:
            candidates = []
            if index < len(requests):
                candidates.append(requests[index].arrival)
            if self.completions:
                candidates.append(self.completions[0].finish)
            if self.release_events:
                candidates.append(self.release_events[0][0])
            if not candidates:
                break
            cycle = max(cycle, min(candidates))
            self.complete_due(cycle)
            self.release_due(cycle)
            while index < len(requests) and requests[index].arrival <= cycle:
                self.ingest(requests[index], cycle)
                index += 1
            queue_depth = len(self.states) + len(self.completions)
            self.stats["max_queue_depth"] = max(int(self.stats["max_queue_depth"]), queue_depth)
            self.issue_ready(cycle)
            if index >= len(requests) and self.states and not self.completions and not self.release_events:
                self.issue_ready(cycle)
                if self.states and not self.completions:
                    raise RuntimeError("replay made no progress; check page ordering")
        requested = int(self.stats["requested_read_bytes"])
        media = int(self.stats["media_read_bytes"])
        summary = {
            **self.stats,
            "cycles": cycle,
            "read_amplification": media / requested if requested else 0.0,
            "channel_utilization": [
                busy / max(1, cycle) for busy in self.stats["channel_busy_ticks"]
            ],
            "read_mode": self.read_mode,
            "write_timeout_policy": self.write_policy,
            "channels": self.channels,
            "subarrays": self.subarrays,
            "page_size": self.page_size,
            "hbf_base": self.base,
            "hbf_span": self.span,
        }
        return self.rows, summary


def write_output(path: Path, rows: list[dict[str, object]]) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    with path.open("w", newline="") as stream:
        writer = csv.DictWriter(stream, fieldnames=SCHEMA, lineterminator="\n")
        writer.writeheader()
        writer.writerows(rows)


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", required=True, type=Path)
    parser.add_argument("--output", required=True, type=Path)
    parser.add_argument("--summary", type=Path)
    parser.add_argument("--base", type=parse_int, default=274877906944)
    parser.add_argument("--span", type=parse_int, default=549755813888)
    parser.add_argument("--page-size", type=parse_int, default=4096)
    parser.add_argument("--channels", type=int, default=16)
    parser.add_argument("--subarrays", type=int, default=256)
    parser.add_argument("--max-active", type=int, default=64)
    parser.add_argument("--t-read", type=int, default=100)
    parser.add_argument("--t-prog", type=int, default=200)
    parser.add_argument("--t-erase", type=int, default=1000)
    parser.add_argument("--read-mode", choices=("demand", "aggregation"), default="demand")
    parser.add_argument("--read-agg-window", type=int, default=0)
    parser.add_argument("--read-agg-threshold", type=parse_int, default=4096)
    parser.add_argument("--write-timeout", type=int, default=1000)
    parser.add_argument("--write-timeout-policy", choices=("partial", "strict"), default="partial")
    parser.add_argument("--channel-bw-gbps", type=float, default=192.0)
    parser.add_argument("--dram-freq-mhz", type=float, default=850.0)
    return parser


def main(argv: Iterable[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    if args.span <= 0 or args.page_size <= 0 or args.span % args.page_size:
        raise SystemExit("span must be positive and divisible by page size")
    requests = read_requests(args.input)
    replay = DeviceReplay(args)
    rows, summary = replay.run(requests)
    write_output(args.output, rows)
    summary_path = args.summary or Path(str(args.output) + ".summary.json")
    summary_path.parent.mkdir(parents=True, exist_ok=True)
    summary_path.write_text(json.dumps(summary, indent=2, sort_keys=True) + "\n")
    print(json.dumps(summary, sort_keys=True))
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, ValueError, RuntimeError) as exc:
        print(f"device_replay.py: {exc}", file=sys.stderr)
        raise SystemExit(2)
