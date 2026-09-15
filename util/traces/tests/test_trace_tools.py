#!/usr/bin/env python3
"""Small regression tests for the trace remap/validation utilities."""

from __future__ import annotations

import csv
import json
import sys
import tempfile
import unittest
from pathlib import Path

HERE = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(HERE))
sys.path.insert(0, str(Path(__file__).resolve().parents[2] / "hbf"))

import remap_global_addresses  # noqa: E402
import validate_hbf_trace  # noqa: E402
import device_replay  # noqa: E402


class TraceToolTests(unittest.TestCase):
    def write_trace(self, root: Path, header: list[str], rows: list[list[str]] | None = None) -> Path:
        trace = root / "trace.csv"
        with trace.open("w", newline="") as stream:
            writer = csv.writer(stream)
            writer.writerow(header)
            writer.writerows(rows or [])
        return trace

    def test_csv_remap_preserves_shared_rows(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "requests.csv"
            target = root / "mapped.csv"
            source.write_text(
                "request_id,op,address,space,bytes\n"
                "1,R,4096,global,64\n"
                "2,R,8192,shared,64\n"
            )
            remap_global_addresses.main(
                [
                    "--input", str(source), "--output", str(target),
                    "--base", "0x100000000", "--span", "0x4000",
                ]
            )
            with target.open() as stream:
                rows = list(csv.DictReader(stream))
            self.assertEqual(rows[0]["address"], str(0x100000000))
            self.assertEqual(rows[1]["address"], "8192")

    def test_trace_validation_detects_duplicate_completion(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            trace = Path(directory) / "trace.csv"
            trace.write_text(
                "sim_cycle,request_id,source_subpartition,op,address,page,channel,subarray,state,bytes,queue_depth,latency,cache_hit,mshr_hit,error\n"
                "0,1,0,R,4096,0,0,0,INGRESS,64,1,0,0,0,\n"
                "10,1,0,R,4096,0,0,1,READ,4096,1,0,0,0,\n"
                "20,1,0,R,4096,0,0,1,COMPLETED,64,0,20,0,0,\n"
                "21,1,0,R,4096,0,0,1,COMPLETED,64,0,21,0,0,\n"
            )
            summary = validate_hbf_trace.validate(trace, 1, 2, 1)
            self.assertFalse(summary["valid"])
            self.assertTrue(any("expected one COMPLETED" in error for error in summary["errors"]))

    def test_trace_validation_rejects_extra_column(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            trace = self.write_trace(
                Path(directory), validate_hbf_trace.SCHEMA + ["future_field"]
            )
            summary = validate_hbf_trace.validate(trace, 1, 1, None)
            self.assertFalse(summary["valid"])
            self.assertFalse(summary["schema_exact"])
            self.assertTrue(any("schema mismatch" in error for error in summary["errors"]))

    def test_trace_validation_rejects_reordered_columns(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            header = validate_hbf_trace.SCHEMA.copy()
            header[0], header[1] = header[1], header[0]
            trace = self.write_trace(Path(directory), header)
            summary = validate_hbf_trace.validate(trace, 1, 1, None)
            self.assertFalse(summary["valid"])
            self.assertFalse(summary["schema_exact"])

    def test_trace_validation_can_require_positive_traffic(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            trace = self.write_trace(Path(directory), validate_hbf_trace.SCHEMA)
            optional = validate_hbf_trace.validate(trace, 1, 1, None)
            required = validate_hbf_trace.validate(
                trace, 1, 1, None, require_nonempty=True
            )
            self.assertTrue(optional["valid"], optional["errors"])
            self.assertFalse(required["valid"])
            self.assertTrue(
                any("no HBF lifecycle traffic" in error for error in required["errors"])
            )

    def test_device_replay_emits_conserved_lifecycle(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "requests.csv"
            target = root / "replay.csv"
            source.write_text(
                "sim_cycle,request_id,source_subpartition,op,address,bytes,state\n"
                "0,1,0,R,4096,64,INGRESS\n"
                "1,2,0,R,4160,64,INGRESS\n"
                "2,3,0,W,8192,4096,INGRESS\n"
            )
            device_replay.main(
                [
                    "--input", str(source), "--output", str(target),
                    "--base", "4096", "--span", "16384", "--channels", "2",
                    "--subarrays", "4", "--max-active", "2", "--t-read", "2",
                    "--t-prog", "3", "--t-erase", "4", "--write-timeout", "0",
                ]
            )
            summary = validate_hbf_trace.validate(target, 2, 4, 3)
            self.assertTrue(summary["valid"], summary["errors"])
            self.assertEqual(summary["ingress_events"], 3)
            self.assertEqual(summary["completion_events"], 3)

    def test_device_replay_strict_partial_write_is_an_error(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source = root / "partial.csv"
            target = root / "strict.csv"
            source.write_text(
                "sim_cycle,request_id,source_subpartition,op,address,bytes,state\n"
                "0,1,0,W,4096,64,INGRESS\n"
            )
            device_replay.main(
                [
                    "--input", str(source), "--output", str(target),
                    "--base", "4096", "--span", "16384", "--channels", "1",
                    "--subarrays", "1", "--t-prog", "3", "--write-timeout", "0",
                    "--write-timeout-policy", "strict",
                ]
            )
            with target.open(newline="") as stream:
                rows = list(csv.DictReader(stream))
            summary = json.loads(target.with_name(target.name + ".summary.json").read_text())
            trace_summary = validate_hbf_trace.validate(target, 1, 1, 1)
            self.assertTrue(trace_summary["valid"], trace_summary["errors"])
            self.assertEqual(summary["incomplete_page_errors"], 1)
            self.assertEqual(summary["page_programs"], 0)
            self.assertEqual([row["state"] for row in rows], ["INGRESS", "COMPLETED"])
            self.assertEqual(rows[-1]["error"], "incomplete_page_policy")

    def test_replay_duplicate_writes_do_not_complete_a_page(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source, target = root / "duplicates.csv", root / "result.csv"
            source.write_text("sim_cycle,request_id,source_subpartition,op,address,bytes,state\n" +
                              "".join(f"0,{i},0,W,4096,64,INGRESS\n" for i in range(64)))
            device_replay.main(["--input", str(source), "--output", str(target),
                               "--base", "4096", "--span", "16384", "--channels", "1",
                               "--subarrays", "1", "--write-timeout", "10",
                               "--write-timeout-policy", "strict"])
            summary = json.loads(Path(str(target) + ".summary.json").read_text())
            self.assertEqual(summary["page_programs"], 0)
            self.assertEqual(summary["incomplete_page_errors"], 1)

    def test_replay_read_forced_flush_checks_write_coverage(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            root = Path(directory)
            source, target = root / "ordering.csv", root / "result.csv"
            source.write_text("sim_cycle,request_id,source_subpartition,op,address,bytes,state\n"
                              "0,1,0,W,4096,64,INGRESS\n1,2,0,R,4096,64,INGRESS\n")
            device_replay.main(["--input", str(source), "--output", str(target),
                               "--base", "4096", "--span", "16384", "--channels", "1",
                               "--subarrays", "1", "--write-timeout", "100",
                               "--write-timeout-policy", "strict"])
            summary = json.loads(Path(str(target) + ".summary.json").read_text())
            self.assertEqual(summary["page_programs"], 0)
            self.assertEqual(summary["incomplete_page_errors"], 1)

    def test_ftl_remap_can_change_physical_subarray(self) -> None:
        with tempfile.TemporaryDirectory() as directory:
            trace = Path(directory) / "remapped.csv"
            trace.write_text(
                "sim_cycle,request_id,source_subpartition,op,address,page,channel,subarray,state,bytes,queue_depth,latency,cache_hit,mshr_hit,error\n"
                "0,1,0,R,4096,0,0,0,INGRESS,64,0,0,0,0,\n"
                "1,1,0,R,4096,0,0,1,READ,4096,1,0,0,0,\n"
                "2,1,0,R,4096,0,0,1,COMPLETED,64,0,2,0,0,\n"
                "3,2,0,W,4096,0,0,2,INGRESS,64,0,0,0,0,\n"
                "4,2,0,W,4096,0,0,2,COMPLETED,64,0,1,0,0,\n"
            )
            summary = validate_hbf_trace.validate(trace, 1, 4, 2)
            self.assertTrue(summary["valid"], summary["errors"])


if __name__ == "__main__":
    unittest.main()
