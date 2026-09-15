#!/usr/bin/env python3
"""Validate complete kernel replay and final aggregate HBF counters."""
from __future__ import annotations
import argparse
import json
from pathlib import Path
import re

PATTERNS = {
    'requests': r'^HBF Total Requests:\s*(\d+)',
    'read_completions': r'^HBF Read Latency:.*\((\d+) reqs\)',
    'all_completions': r'^--------- HBF Stack 0 \(routed=\d+ completed=(\d+)\)',
    'page_reads': r'^HBF Page Reads:\s*(\d+)',
    'buffer_hits': r'^HBF Page Buffer Hits:\s*(\d+)',
    'page_programs': r'^HBF Page Programs:\s*(\d+)',
    'returned_sector_bytes': r'^HBF Bytes Read:\s*(\d+)',
}


def validate(run: Path, traces: Path, *, require_rc: bool = True) -> dict:
    if require_rc and (run / 'run.rc').read_text().strip() != '0':
        raise ValueError('simulator failed')
    expected = sum(x.startswith('kernel') for x in (traces / 'kernelslist.g').read_text().splitlines())
    totals = {}
    completed = cycles = 0
    with (run / 'run.log').open(errors='replace') as stream:
        for line in stream:
            if re.match(r'^gpu_sim_cycle\s*=', line):
                completed += 1
            match = re.match(r'^gpu_tot_sim_cycle\s*=\s*(\d+)', line)
            if match:
                cycles = int(match[1])
            if line.startswith('========= HBF System Statistics ========='):
                totals = {}
            for key, pattern in PATTERNS.items():
                match = re.match(pattern, line)
                if match:
                    totals[key] = int(match[1])
    if expected == 0 or completed != expected:
        raise ValueError(f'kernel completion mismatch: {completed}/{expected}')
    if totals.keys() != PATTERNS.keys() or cycles <= 0:
        raise ValueError('missing final counters')
    if not (totals['requests'] == totals['read_completions'] == totals['all_completions'] > 0):
        raise ValueError('read request conservation failed')
    if totals['page_programs'] != 0 or totals['returned_sector_bytes'] != totals['read_completions'] * 32:
        raise ValueError('unexpected write traffic or sector width')
    if totals['buffer_hits'] > totals['page_reads']:
        raise ValueError('invalid page-service counters')
    config = dict(re.findall(r'^-(\S+)\s+([^\n#]+)', (run / 'gpgpusim.config').read_text(), re.M))
    if int(config['gpgpu_hbf_num_stacks']) != 1:
        raise ValueError('Qwen reference uses one stack')
    mhz = float(config['gpgpu_clock_domains'].split(':')[0])
    totals.update(
        cycles=cycles, captured_kernels=expected, completed_kernels=completed,
        milliseconds=cycles / mhz / 1000,
        array_read_bytes=(totals['page_reads'] - totals['buffer_hits']) * 4096,
        validation_scope='final aggregate counters; not per-request lifecycle validation',
    )
    return totals


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--run', required=True, type=Path)
    parser.add_argument('--traces', required=True, type=Path)
    parser.add_argument('--output', required=True, type=Path)
    args = parser.parse_args()
    report = validate(args.run, args.traces)
    args.output.write_text(json.dumps(report, indent=2) + '\n')
    print(json.dumps(report, indent=2))
