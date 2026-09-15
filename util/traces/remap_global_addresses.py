#!/usr/bin/env python3
"""Map global-memory addresses into a configured HBF address span.

The tool keeps trace metadata and non-global addresses unchanged.  CSV is the
preferred interchange format because it makes the address-space decision
explicit.  Accel-Sim ``.traceg`` files are also supported for the common
LDG/STG form; generic LD/ST instructions require ``--assume-global``.
"""

from __future__ import annotations

import argparse
import csv
import hashlib
import json
import re
import sys
from pathlib import Path
from typing import Iterable

PAGE_DEFAULT = 4096
HEX_RE = re.compile(r"^0[xX][0-9a-fA-F]+$")
GLOBAL_OP_RE = re.compile(r"^(LDG|STG)(?:\.|$)", re.IGNORECASE)
GENERIC_OP_RE = re.compile(r"^(LD|ST)(?:\.|$)", re.IGNORECASE)
SHARED_OP_RE = re.compile(r"^(LDS|STS|LD\.SHARED|ST\.SHARED)", re.IGNORECASE)


def sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def parse_int(value: str) -> int:
    return int(value.strip(), 0)


def is_global_space(value: str | None) -> bool:
    if value is None:
        return False
    return value.strip().lower() in {"global", "gmem", "global_memory", "global-memory"}


def classify_op(op: str, assume_global: bool) -> str:
    op = op.strip()
    if GLOBAL_OP_RE.match(op):
        return "global"
    if SHARED_OP_RE.match(op):
        return "shared"
    if GENERIC_OP_RE.match(op):
        return "global" if assume_global else "ambiguous"
    return "other"


def remap_address(address: int, source_base: int, base: int, span: int, wrap: bool) -> int:
    relative = address - source_base
    if relative < 0:
        raise ValueError(f"address 0x{address:x} precedes source base 0x{source_base:x}")
    if relative >= span:
        if not wrap:
            raise ValueError(
                f"address 0x{address:x} exceeds HBF span ({relative} >= {span}); "
                "use --wrap to make aliasing explicit"
            )
        relative %= span
    return base + relative


def detect_csv_global(row: dict[str, str], assume_global: bool) -> bool:
    for key in ("space", "address_space", "memory_space", "addr_space"):
        if key in row and row[key] != "":
            return is_global_space(row[key])
    classification = classify_op(row.get("op", ""), assume_global)
    return classification == "global"


def remap_csv(
    input_path: Path,
    output_path: Path,
    base: int,
    span: int,
    page_size: int,
    source_base_arg: int | None,
    assume_global: bool,
    wrap: bool,
) -> dict:
    with input_path.open(newline="") as stream:
        reader = csv.DictReader(stream)
        if reader.fieldnames is None or "address" not in reader.fieldnames:
            raise ValueError("CSV input must contain an address column")
        rows = list(reader)

    global_addresses = []
    for row in rows:
        if detect_csv_global(row, assume_global):
            try:
                global_addresses.append(parse_int(row["address"]))
            except (KeyError, ValueError) as exc:
                raise ValueError(f"invalid global address in row {row}") from exc
    if not global_addresses:
        source_base = source_base_arg if source_base_arg is not None else 0
    else:
        source_base = (
            source_base_arg
            if source_base_arg is not None
            else min(global_addresses) // page_size * page_size
        )

    output_path.parent.mkdir(parents=True, exist_ok=True)
    with output_path.open("w", newline="") as stream:
        writer = csv.DictWriter(stream, fieldnames=reader.fieldnames, lineterminator="\n")
        writer.writeheader()
        for row in rows:
            if detect_csv_global(row, assume_global):
                old = parse_int(row["address"])
                new = remap_address(old, source_base, base, span, wrap)
                row["address"] = str(new)
                if "page" in row and row["page"] != "":
                    row["page"] = str((new - base) // page_size)
            writer.writerow(row)

    return {
        "format": "csv",
        "rows": len(rows),
        "global_rows": len(global_addresses),
        "source_address_min": min(global_addresses) if global_addresses else None,
        "source_address_max": max(global_addresses) if global_addresses else None,
        "source_base": source_base,
        "hbf_base": base,
        "hbf_span": span,
        "page_size": page_size,
        "hbf_pages": span // page_size,
        "wrapped": bool(wrap),
    }


def traceg_op_and_addresses(tokens: list[str], assume_global: bool) -> tuple[str, list[int]]:
    if len(tokens) < 8:
        return "other", []
    try:
        dest_count = int(tokens[6], 0)
    except ValueError:
        return "other", []
    opcode_index = 7 + dest_count
    if opcode_index >= len(tokens):
        return "other", []
    op = tokens[opcode_index]
    classification = classify_op(op, assume_global)
    if classification != "global":
        return classification, []
    src_count_index = opcode_index + 1
    try:
        src_count = int(tokens[src_count_index], 0)
    except (IndexError, ValueError):
        return classification, []
    mem_width_index = src_count_index + 1 + src_count
    if mem_width_index >= len(tokens):
        return classification, []
    addresses = []
    for token in tokens[mem_width_index + 1 :]:
        if HEX_RE.match(token):
            addresses.append(int(token, 16))
    return classification, addresses


def remap_traceg(
    input_path: Path,
    output_path: Path,
    base: int,
    span: int,
    page_size: int,
    source_base_arg: int | None,
    assume_global: bool,
    wrap: bool,
) -> dict:
    lines = input_path.read_text(encoding="utf-8", errors="replace").splitlines()
    parsed: list[tuple[list[str] | None, str, list[int]]] = []
    global_addresses: list[int] = []
    for line in lines:
        stripped = line.strip()
        if not stripped or stripped.startswith("#") or stripped.startswith("-"):
            parsed.append((None, line, []))
            continue
        tokens = stripped.split()
        classification, addresses = traceg_op_and_addresses(tokens, assume_global)
        parsed.append((tokens, classification, addresses))
        if classification == "global":
            global_addresses.extend(addresses)
    source_base = (
        source_base_arg
        if source_base_arg is not None
        else (min(global_addresses) // page_size * page_size if global_addresses else 0)
    )

    out_lines = []
    remapped_count = 0
    for tokens, classification, addresses in parsed:
        if tokens is None:
            out_lines.append(classification)
            continue
        if classification == "global" and addresses:
            address_index = 0
            # Recompute the address suffix index using the same traceg grammar.
            dest_count = int(tokens[6], 0)
            opcode_index = 7 + dest_count
            src_count = int(tokens[opcode_index + 1], 0)
            first_address = opcode_index + 1 + 1 + src_count + 1
            for index in range(first_address, len(tokens)):
                if HEX_RE.match(tokens[index]):
                    tokens[index] = hex(
                        remap_address(int(tokens[index], 16), source_base, base, span, wrap)
                    )
                    remapped_count += 1
        out_lines.append(" ".join(tokens))
    output_path.parent.mkdir(parents=True, exist_ok=True)
    output_path.write_text("\n".join(out_lines) + "\n", encoding="utf-8")
    return {
        "format": "traceg",
        "rows": len(lines),
        "global_addresses": len(global_addresses),
        "remapped_addresses": remapped_count,
        "source_address_min": min(global_addresses) if global_addresses else None,
        "source_address_max": max(global_addresses) if global_addresses else None,
        "source_base": source_base,
        "hbf_base": base,
        "hbf_span": span,
        "page_size": page_size,
        "hbf_pages": span // page_size,
        "wrapped": bool(wrap),
    }


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", required=True, type=Path)
    parser.add_argument("--output", required=True, type=Path)
    parser.add_argument("--base", required=True, type=parse_int, help="HBF base address")
    parser.add_argument("--span", required=True, type=parse_int, help="HBF address span in bytes")
    parser.add_argument("--page-size", type=parse_int, default=PAGE_DEFAULT)
    parser.add_argument("--source-base", type=parse_int)
    parser.add_argument(
        "--assume-global",
        action="store_true",
        help="treat metadata-free generic LD/ST as global memory",
    )
    parser.add_argument(
        "--wrap",
        action="store_true",
        help="wrap addresses beyond the span; the manifest records aliasing",
    )
    parser.add_argument("--manifest", type=Path, help="manifest path (default: OUTPUT.manifest.json)")
    return parser


def main(argv: Iterable[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    if args.span <= 0 or args.page_size <= 0 or args.span % args.page_size:
        raise SystemExit("--span must be positive and divisible by --page-size")
    suffix = args.input.suffix.lower()
    if suffix == ".csv":
        summary = remap_csv(
            args.input,
            args.output,
            args.base,
            args.span,
            args.page_size,
            args.source_base,
            args.assume_global,
            args.wrap,
        )
    elif suffix == ".traceg":
        summary = remap_traceg(
            args.input,
            args.output,
            args.base,
            args.span,
            args.page_size,
            args.source_base,
            args.assume_global,
            args.wrap,
        )
    else:
        raise SystemExit("input format must be .csv or .traceg")
    manifest_path = args.manifest or Path(str(args.output) + ".manifest.json")
    manifest = {
        "input": str(args.input.resolve()),
        "output": str(args.output.resolve()),
        "input_sha256": sha256(args.input),
        "output_sha256": sha256(args.output),
        **summary,
    }
    manifest_path.parent.mkdir(parents=True, exist_ok=True)
    manifest_path.write_text(json.dumps(manifest, indent=2, sort_keys=True) + "\n")
    print(json.dumps(manifest, sort_keys=True))
    return 0


if __name__ == "__main__":
    try:
        raise SystemExit(main())
    except (OSError, ValueError) as exc:
        print(f"remap_global_addresses.py: {exc}", file=sys.stderr)
        raise SystemExit(2)
