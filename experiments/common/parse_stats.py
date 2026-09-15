#!/usr/bin/env python3
"""experiments/common/parse_stats.py — 从 run.log 提取统一统计

用法:
  parse_stats.py --csv-header           # 打印 CSV 表头
  parse_stats.py --csv <log> [meta]     # 打印一行 CSV（meta 为可选前置列）
  parse_stats.py <log>                  # 打印 key=value 行（调试用）

统计口径: 新版日志取最后一个 HBF system 快照并跨 stack 聚合；旧版
logical-cube 日志取最后一个 cube 快照，避免按 GPU memory partition 重复。
更早的 per-partition 日志保留求和兼容路径。cycles 取末尾 gpu_sim_cycle。
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
    "hbf_channels", "hbf_subarrays", "max_queue_depth", "cycles_active",
    "partial_page_programs", "incomplete_page_errors", "padding_bytes",
    "write_timeout_flushes", "idle_drain_flushes", "requested_read_bytes",
    "media_read_bytes", "read_amplification", "aggregation_wait",
    "first_request_latency", "first_request_pages", "avg_op_latency",
    "avg_read_latency", "max_read_latency", "allocated_metadata_pages",
    "active_subarrays", "active_subarrays_total", "capacity_error",
    "channel_util_min", "channel_util_avg", "channel_util_max",
    "channel_imbalance",
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
    "capacity_used_pages":  (re.compile(r"HBF Capacity Usage:\s+" + NUM + r"(?:\s+live)?\s+/\s+"), "sum"),
    "capacity_total_pages": (re.compile(r"HBF Capacity Usage:\s+" + NUM + r"(?:\s+live)?\s+/\s+" + NUM), "sum", 1),
    "capacity_pct":      (re.compile(r"HBF Capacity Usage:.*?\(" + FLOAT + r"%\)"), "sum_float"),
    "logical_pages":     (re.compile(r"HBF Logical Pages:\s+" + NUM), "sum"),
    "subarrays_used":    (re.compile(r"HBF Subarray Spread:\s+" + NUM + r" / "), "sum"),
    "subarrays_total":   (re.compile(r"HBF Subarray Spread:\s+" + NUM + r" / " + NUM), "sum", 1),
    "erase_min":         (re.compile(r"HBF FTL Erase Count:\s+min=" + NUM), "sum"),
    "erase_avg":         (re.compile(r"HBF FTL Erase Count:\s+min=" + NUM + r" avg=" + NUM), "sum", 1),
    "erase_max":         (re.compile(r"HBF FTL Erase Count:\s+min=" + NUM + r" avg=" + NUM + r" max=" + NUM), "sum", 2),
    "dram_accesses":     (re.compile(r"gpu_total_dram_accesses = " + NUM), "last"),
    "hbf_channels":      (re.compile(r"HBF Logical Cube Channels:\s+" + NUM), "cube_last"),
    "hbf_subarrays":     (re.compile(r"HBF Sub-arrays:\s+" + NUM), "cube_last"),
    "max_queue_depth":   (re.compile(r"HBF Max Queue Depth:\s+" + NUM), "cube_last"),
    "cycles_active":     (re.compile(r"HBF Cycles Active:\s+" + NUM), "cube_last"),
    "partial_page_programs": (re.compile(r"HBF Partial Page Programs:\s+" + NUM), "cube_last"),
    "incomplete_page_errors": (re.compile(r"HBF Incomplete Page Errors:\s+" + NUM), "cube_last"),
    "padding_bytes":     (re.compile(r"HBF Padding Bytes:\s+" + NUM), "cube_last"),
    "write_timeout_flushes": (re.compile(r"HBF Write Timeout Flushes:\s+" + NUM), "cube_last"),
    "idle_drain_flushes": (re.compile(r"HBF Idle Drain (?:Events|Flushes):\s+" + NUM), "cube_last"),
    "requested_read_bytes": (re.compile(r"HBF Requested Read Bytes:\s+" + NUM), "cube_last"),
    "media_read_bytes": (re.compile(r"HBF Media Read Bytes:\s+" + NUM), "cube_last"),
    "read_amplification": (re.compile(r"HBF Read Amplification:\s+" + FLOAT), "cube_last_float"),
    "aggregation_wait": (re.compile(r"HBF Aggregation Wait:\s+" + NUM), "cube_last"),
    "first_request_latency": (re.compile(r"HBF First-Request Latency:\s+" + NUM), "cube_last"),
    "first_request_pages": (re.compile(r"HBF First-Request Latency:\s+" + NUM + r" core cycles \(" + NUM + r" pages\)"), "cube_last", 1),
    "avg_op_latency": (re.compile(r"HBF Avg Op Latency:\s+" + NUM), "cube_last"),
    "avg_read_latency": (re.compile(r"HBF Read Latency:\s+avg=" + NUM), "cube_last"),
    "max_read_latency": (re.compile(r"HBF Read Latency:\s+avg=" + NUM + r" max=" + NUM), "cube_last", 1),
    "allocated_metadata_pages": (re.compile(r"HBF Allocated Metadata:\s+" + NUM), "cube_last"),
    "active_subarrays": (re.compile(r"HBF Active Sub-arrays:\s+" + NUM + r" / "), "cube_last"),
    "active_subarrays_total": (re.compile(r"HBF Active Sub-arrays:\s+" + NUM + r" / " + NUM), "cube_last", 1),
    "channel_util_min": (re.compile(r"HBF Channel Utilization:\s+min=" + FLOAT + r"%"), "cube_last_float"),
    "channel_util_avg": (re.compile(r"HBF Channel Utilization:\s+min=" + FLOAT + r"% avg=" + FLOAT + r"%"), "cube_last_float", 1),
    "channel_util_max": (re.compile(r"HBF Channel Utilization:\s+min=" + FLOAT + r"% avg=" + FLOAT + r"% max=" + FLOAT + r"%"), "cube_last_float", 2),
    "channel_imbalance": (re.compile(r"HBF Channel Utilization:.*?imbalance=" + FLOAT), "cube_last_float"),
}

CAPACITY_ERROR = re.compile(r"HBF FTL Capacity Error:\s+(yes|no)")
HBF_SYSTEM_MARKER = "========= HBF System Statistics ========="
HBF_CUBE_MARKER = "========= HBF Cube Controller Statistics ========="
READ_LATENCY = re.compile(
    r"HBF Read Latency:\s+avg=" + NUM + r" max=" + NUM +
    r" core cycles \(" + NUM + r" reqs\)"
)

SYSTEM_MIN_FIELDS = {"erase_min", "channel_util_min"}
SYSTEM_MAX_FIELDS = {
    "erase_max", "max_queue_depth", "max_read_latency", "channel_util_max",
}


def _hbf_snapshot(log_text):
    system_start = log_text.rfind(HBF_SYSTEM_MARKER)
    if system_start >= 0:
        snapshot = log_text[system_start:]
        return snapshot, "system", snapshot.split(HBF_CUBE_MARKER)[1:]
    cube_start = log_text.rfind(HBF_CUBE_MARKER)
    if cube_start >= 0:
        return log_text[cube_start:], "cube", []
    return log_text, "legacy", []


def _field_value(block, name):
    spec = PATTERNS[name]
    match = spec[0].search(block)
    if match is None:
        return None
    group = spec[2] if len(spec) > 2 else 0
    value = match.group(group + 1)
    return float(value) if "float" in spec[1] else int(value)


def _weighted_controller_mean(blocks, value_name, weight_fn):
    weighted_sum = 0
    total_weight = 0
    for block in blocks:
        value = _field_value(block, value_name)
        weight = weight_fn(block)
        if value is None or weight <= 0:
            continue
        weighted_sum += value * weight
        total_weight += weight
    return weighted_sum / total_weight if total_weight else 0


def _count(block, name):
    value = _field_value(block, name)
    return value if value is not None else 0


def parse(log_text):
    out = {}
    hbf_log, log_kind, controller_blocks = _hbf_snapshot(log_text)
    for name, spec in PATTERNS.items():
        if spec is None:
            continue
        pat, mode = spec[0], spec[1]
        group = spec[2] if len(spec) > 2 else 0
        source = log_text if mode == "last" else hbf_log
        matches = list(pat.finditer(source))
        is_float = mode in {"cube_last_float", "sum_float"}
        values = [
            float(m.group(group + 1)) if is_float else int(m.group(group + 1))
            for m in matches
        ]
        if mode == "last" or log_kind == "cube":
            out[name] = values[-1] if values else 0
        elif log_kind == "system":
            if name in SYSTEM_MIN_FIELDS:
                out[name] = min(values) if values else 0
            elif name in SYSTEM_MAX_FIELDS:
                out[name] = max(values) if values else 0
            else:
                out[name] = sum(values)
        elif mode in {"cube_last", "cube_last_float"}:
            out[name] = values[-1] if values else 0
        else:
            out[name] = sum(values)

    if log_kind == "system":
        used_pages = out.get("capacity_used_pages", 0)
        total_pages = out.get("capacity_total_pages", 0)
        out["capacity_pct"] = (
            100.0 * used_pages / total_pages if total_pages else 0.0
        )
        requested_bytes = out.get("requested_read_bytes", 0)
        out["read_amplification"] = (
            out.get("media_read_bytes", 0) / requested_bytes
            if requested_bytes else 0.0
        )
        out["erase_avg"] = int(_weighted_controller_mean(
            controller_blocks, "erase_avg",
            lambda block: _count(block, "capacity_total_pages"),
        ))
        out["avg_op_latency"] = int(_weighted_controller_mean(
            controller_blocks, "avg_op_latency",
            lambda block: _count(block, "hbf_requests"),
        ))

        read_latency_sum = 0
        read_requests = 0
        for block in controller_blocks:
            match = READ_LATENCY.search(block)
            if match is None:
                continue
            requests = int(match.group(3))
            read_latency_sum += int(match.group(1)) * requests
            read_requests += requests
        out["avg_read_latency"] = (
            read_latency_sum // read_requests if read_requests else 0
        )

        out["channel_util_avg"] = _weighted_controller_mean(
            controller_blocks, "channel_util_avg",
            lambda block: _count(block, "hbf_channels"),
        )
        channel_avg = out["channel_util_avg"]
        out["channel_imbalance"] = (
            (out.get("channel_util_max", 0.0) -
             out.get("channel_util_min", 0.0)) / channel_avg
            if channel_avg else 0.0
        )

    capacity = CAPACITY_ERROR.findall(hbf_log)
    if capacity:
        out["capacity_error"] = int(any(value == "yes" for value in capacity))
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
