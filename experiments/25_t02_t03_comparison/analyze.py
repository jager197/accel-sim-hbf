#!/usr/bin/env python3
"""Validate frozen experiments and report descriptive abstraction differences."""
from __future__ import annotations
import csv,json,hashlib,shutil,subprocess,xml.etree.ElementTree as ET
from pathlib import Path
from run_replay import ROOT,HERE,FULL,digest,events,measured

def main()->None:
    prov=json.loads((FULL/'provenance.json').read_text())
    lib=next((ROOT/'gpu-simulator/gpgpu-sim/lib').glob('gcc-*/cuda-*/release/libcudart.so'))
    assert digest(lib)==prov['simulator_sha256']
    for name,expected in prov['hbf_sources'].items():
        assert digest(ROOT/'hbf'/name)==expected
        assert digest(ROOT/'gpu-simulator/gpgpu-sim/src/gpgpu-sim'/name)==expected
    checks=json.loads((HERE/'self_validation.json').read_text());assert len(checks)==24 and all(c['exact'] for c in checks)
    with (HERE/'replay_summary.csv').open() as f:rows=list(csv.DictReader(f))
    assert len(rows)==24
    gains=[];lookup={r['name']:r for r in rows}
    for r in rows:
        assert r['trace_sha256']==digest(HERE/'replays'/('cross_'+r['name'])/'hbf.csv')
        if int(r['mshr'])!=1:continue
        off=lookup[r['name'][:-1]+'0'];base=float(off['full_span'])
        full=100*(1-float(r['full_span'])/base);replay=100*(1-float(r['replay_span'])/base)
        gains.append(dict(entries=int(r['entries']),depth=int(r['depth']),tr_us=int(r['tr'])/850,baseline_cycles=int(base),full_cycles=int(r['full_span']),replay_cycles=int(r['replay_span']),full_gain_pct=full,replay_gain_pct=replay,gain_gap_pp=full-replay))
    with (HERE/'gain_summary.csv').open('w',newline='') as f:w=csv.DictWriter(f,fieldnames=list(gains[0]));w.writeheader();w.writerows(gains)
    with (HERE/'external_summary.csv').open() as f:external=list(csv.DictReader(f))
    assert len(external)==8
    for r in external:
        out=HERE/'external'/r['name'];tree=ET.parse(out/'workload_scenario_1.xml')
        transactions=[n for n in tree.iter() if 'TSU.User_Read_TR_Queue' in n.tag]
        assert sum(int(n.attrib['No_Of_Transactions_Dequeued']) for n in transactions)==64
        logs=(out/'hbf.log').read_text()
        assert 'HBF Page Reads:         64' in logs
        assert 'HBF Page Buffer Hits:   0' in logs
        if r['arrival']=='burst':
            lower=64/int(r['channels'])*float(r['tr_us'])*1000
            assert float(r['hbf_span_ns'])>=lower
            assert float(r['mqsim_span_ns'])>=lower
    for ch in(1,4):
        pair={int(r['tr_us']):r for r in external if int(r['channels'])==ch and r['arrival']=='burst'}
        for k in('hbf_span_ns','mqsim_span_ns'):
            assert abs(float(pair[15][k])-float(pair[1][k])-64/ch*14000)<0.01
    lines=['# T02/T03 analysis','', '## Protocol and counts','',
           '24 fresh execution-driven GPU runs, 24 exact identity replays, 24 opposite-MSHR frozen-ingress counterfactuals, and 8 MQSim/HBF external pairs. All runs are deterministic. Input multisets and completion conservation are checked. Full and Replay share the installed C++ HBF implementation.', '',
           '## MSHR benefit with 32 entries per page','',
           'Window = first HBF admission to last HBF completion. Gain = 100*(1 - enabled window / disabled Full window). Replay freezes the disabled Full ingress schedule for both states; its disabled identity replay is exact. No GPU kernel time is assigned to Replay.','',
           '| tR (us) | admission cap | Full gain (%) | Replay gain (%) | difference (pp) |','|---:|---:|---:|---:|---:|']
    for g in gains:
        if g['entries']==32:lines.append(f"| {g['tr_us']:g} | {g['depth']} | {g['full_gain_pct']:.3f} | {g['replay_gain_pct']:.3f} | {g['gain_gap_pp']:.3f} |")
    lines+=['','## External MQSim burst comparison','','| channels | tR (us) | HBF window (us) | MQSim window (us) | difference (%) |','|---:|---:|---:|---:|---:|']
    for r in external:
        if r['arrival']=='burst':lines.append(f"| {r['channels']} | {r['tr_us']} | {float(r['hbf_span_ns'])/1000:.3f} | {float(r['mqsim_span_ns'])/1000:.3f} | {float(r['gap_pct']):.2f} |")
    lines+=['','## Interpretation and claim boundaries','',
      '- Supported: a frozen controller-ingress trace can retain GPU-side admission delays from its source configuration and miss the benefit of enabling merging. At 15 us/cap128, it predicts 0.816% versus 61.118% reduction in the detailed model. Large-capacity controls agree; reverse transfers and sparse controls are in the full CSV.',
      '- Supported: MQSim and HBF reproduce the same 14-us/page increase in burst completion (896 us with one channel,224 us with four). Their absolute windows differ because interfaces and protocol overheads remain different. This is an external cross-model check of read service and scaling, not a silicon calibration or a full GPU/SSD comparison.',
      '- Unsupported: every GPU pipeline detail is necessary; hardware accuracy of either model; all SSD/LLM simulators fail; independent page-model accuracy from MSHR switching alone; policy ranking reversal (not established).',
      '- Frozen ingress removes the entire source feedback path, including upstream admission/credit stalls; this does not isolate just warp scheduling. FIFO boundary delivery is documented in replay.cc; identity checks validate it at all 24 source points, not for arbitrary workloads.',
      '- External workload:64 unique full pages, represented as one8-sector NVMe read in MQSim and32x128B HBF requests. All payload bytes match; HBF has64 page services. Caches off, ideal MQSim mapping, read-only (no GC/writes), one serial die/subarray per channel. Nominal192GB/s per channel is a deliberately synthetic common rate, with MQSim192-byte bus at1000MT/s and768GB/s PCIe aggregate. Geometry outside the touched pages and device-management contracts are not matched.',
      '- MQSim execution-utilization includes its default20ns ready delay; HBF nominal array duty factor is analytically derived and is not a matching measured utilization statistic. They are excluded from paper accuracy comparisons. MQSim mean latencies are reported in integer microseconds; do not infer sub-us accuracy from them.',
      '- Isolated windows are dominated by50-us interarrival gaps and are not used to claim close accuracy. The burst comparisons and timing slope carry the external quantitative claim.',
      '', '## Evidence manifest', '', 'sources.sha256 records scripts, outputs, HBF sources, Full provenance, and paper source snapshots. MQSim repository commit and binary hash are in external_provenance.json.']
    (HERE/'analysis-report.md').write_text('\n'.join(lines)+'\n')
    (HERE/'stats-appendix.md').write_text('# Statistical scope\n\nOne deterministic run per configuration; no random sampling, confidence intervals, or significance tests. Requests within a run are not independent experimental replicates. All24 paired points retained;12 forward benefits and12 reverse results. Ratios are conditional on selected parameters and an implemented reference model, not hardware error. Percentiles use sorted[(n-1)*95//100]. External spans are reconstructed from unmodified MQSim IOPS and completed request count (six-decimal IOPS serialization causes <0.01ns numerical uncertainty here). All64 external requests serviced in each run.\n')
    (HERE/'figure-catalog.md').write_text('# Figures\n\n- mshr_feedback.pdf: paired MSHR completion-window gains at1/15us over three admission caps. Shows a large frozen-input gap at limited admission and agreement at4096. Dense workload only; sparse controls remain in CSV. No error bars: deterministic runs.\n- mqsim_crosscheck.pdf: external burst durations by channel count and read timing. Shows close long-read scaling and stronger short-read protocol effects. Different interfaces; no accuracy claim. Companion exact table is in analysis-report.md.\n')
    (HERE/'validation.json').write_text(json.dumps(dict(full_runs=24,exact_controller_trace_replays=24,frozen_ingress_counterfactuals=24,external_pairs=8,request_multisets_and_conservation=True,mqsim_transactions_per_pair=64,independent_timing_slope_pass=True,unchanged_hbf_library_and_sources=True),indent=2)+'\n')
    print('validated24 Full/replay identities,24 transfers,8 external pairs and independent timing-slope checks')
if __name__=='__main__':main()
