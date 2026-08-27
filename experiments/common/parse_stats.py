#!/usr/bin/env python3
"""experiments/common/parse_stats.py — 从 run.log 提取统一统计

用法:
  parse_stats.py --csv-header           # 打印 CSV 表头
  parse_stats.py --csv <log> [meta]     # 打印一行 CSV（meta 为可选前置列）
  parse_stats.py <log>                  # 打印 key=value 行（调试用）

统计口径: 各 memory partition 各打印一份 HBF 统计块，这里跨 partition 求和；
cycles 取日志末尾的 gpu_sim_cycle。
"""
import re
import sys

NUM = r"([0-9]+)"
FLOAT = r"([0-9.]+)"

FIELDS = [
    "cycles", "hbf_requests", "mshr_hits", "page_reads", "page_programs",
    "block_erases", "bytes_read", "bytes_written", "cache_hits",
    "cache_misses", "cache_evictions", "page_buffer_hits",
    "page_buffer_misses", "ftl_translations", "ftl_allocations",
    "gc_events", "gc_page_copies", "gc_block_erases", "gc_stall_cycles",
    "capacity_used_pages", "capacity_total_pages", "capacity_pct",
    "logical_pages", "subarrays_used", "subarrays_total",
    "erase_min", "erase_avg", "erase_max", "dram_accesses",
]

PATTERNS = {
    "cycles":            (re.compile(r"gpu_sim_cycle = " + NUM), "last"),
    "hbf_requests":      (re.compile(r"HBF Total Requests:\s+" + NUM), "sum"),
    "mshr_hits":         (re.compile(r"HBF MSHR Hits:\s+" + NUM), "sum"),
    "page_reads":        (re.compile(r"HBF Page Reads:\s+" + NUM), "sum"),
    "page_programs":     (re.compile(r"HBF Page Programs:\s+" + NUM), "sum"),
    "block_erases":      (re.compile(r"HBF Block Erases:\s+" + NUM), "sum"),
    "bytes_read":        (re.compile(r"HBF Bytes Read:\s+" + NUM), "sum"),
    "bytes_written":     (re.compile(r"HBF Bytes Written:\s+" + NUM), "sum"),
    "cache_hits":        (re.compile(r"hits=" + NUM), "sum"),
    "cache_misses":      (re.compile(r"misses=" + NUM), "sum"),
    "cache_evictions":   (re.compile(r"Evictions: " + NUM), "sum"),
    "page_buffer_hits":  (re.compile(r"HBF Page Buffer Hits:\s+" + NUM), "sum"),
    "page_buffer_misses": None,  # 通过命中率反推困难，跳过
    "ftl_translations":  (re.compile(r"HBF FTL Translations:\s+" + NUM), "sum"),
    "ftl_allocations":   (re.compile(r"HBF FTL Allocations:\s+" + NUM), "sum"),
    "gc_events":         (re.compile(r"HBF FTL GC Events:\s+" + NUM), "sum"),
    "gc_page_copies":    (re.compile(r"HBF FTL GC Page Copies:\s+" + NUM), "sum"),
    "gc_block_erases":   (re.compile(r"HBF FTL GC Erases:\s+" + NUM), "sum"),
    "gc_stall_cycles":   (re.compile(r"HBF FTL GC Stall Cycles:" + NUM), "sum"),
    "capacity_used_pages":  (re.compile(r"HBF Capacity Usage:\s+" + NUM + r" / "), "sum"),
    "capacity_total_pages": (re.compile(r"HBF Capacity Usage:\s+" + NUM + r" / " + NUM), "sum", 1),
    "capacity_pct":      (re.compile(r"HBF Capacity Usage:.*?\(" + FLOAT + r"%\)"), "sum_float"),
    "logical_pages":     (re.compile(r"HBF Logical Pages:\s+" + NUM), "sum"),
    "subarrays_used":    (re.compile(r"HBF Subarray Spread:\s+" + NUM + r" / "), "sum"),
    "subarrays_total":   (re.compile(r"HBF Subarray Spread:\s+" + NUM + r" / " + NUM), "sum", 1),
    "erase_min":         (re.compile(r"HBF FTL Erase Count:\s+min=" + NUM), "sum"),
    "erase_avg":         (re.compile(r"HBF FTL Erase Count:\s+min=" + NUM + r" avg=" + NUM), "sum", 1),
    "erase_max":         (re.compile(r"HBF FTL Erase Count:\s+min=" + NUM + r" avg=" + NUM + r" max=" + NUM), "sum", 2),
    "dram_accesses":     (re.compile(r"gpu_total_dram_accesses = " + NUM), "last"),
}


def parse(log_text):
    out = {}
    for name, spec in PATTERNS.items():
        if spec is None:
            continue
        pat, mode = spec[0], spec[1]
        group = spec[2] if len(spec) > 2 else 0
        matches = list(pat.finditer(log_text))
        if mode == "sum_float":
            out[name] = sum(float(m.group(group + 1)) for m in matches)
        else:
            vals = [int(m.group(group + 1)) for m in matches]
            if mode == "last":
                out[name] = vals[-1] if vals else 0
            else:  # sum
                out[name] = sum(vals)
    return out


def main():
    args = sys.argv[1:]
    if args and args[0] == "--csv-header":
        print(",".join(FIELDS))
        return
    if args and args[0] == "--csv":
        log_path = args[1]
        meta = args[2:]
        with open(log_path) as f:
            s = parse(f.read())
        row = [str(s.get(k, 0)) for k in FIELDS]
        print(",".join(meta + row))
        return
    if args:
        with open(args[0]) as f:
            s = parse(f.read())
        for k in FIELDS:
            print(f"{k}={s.get(k, 0)}")
        return
    print("usage: parse_stats.py [--csv-header | --csv <log> [meta...] | <log>]")
    sys.exit(2)


if __name__ == "__main__":
    main()
