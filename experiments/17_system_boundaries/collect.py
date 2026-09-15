#!/usr/bin/env python3
"""Revalidate immutable cases and assemble a declared comparison family."""
from __future__ import annotations
import argparse
import csv
import hashlib
import json
from pathlib import Path
from run import analyze, matrix, digest, infeasible_outcome


def collect(bundles:list[Path],suite:str,output:Path)->list[dict]:
    expected={c['name']:c for c in matrix() if c['suite']==suite}
    rows=[];sources={}
    infeasible_name='phase_w16_d113200_r3_s1'
    for name,case in expected.items():
        candidates=[b for b in bundles if (b/name/'summary.json').exists() or (name==infeasible_name and (b/name/'run.rc').exists())]
        if len(candidates)!=1:raise ValueError(f'{name}: expected exactly one successful source, got {candidates}')
        b=candidates[0]
        declared={c['name']:c for c in json.loads((b/'plan.json').read_text())}
        if declared.get(name)!=case:raise ValueError('plan mismatch '+name)
        if name==infeasible_name and (b/name/'run.rc').read_text().strip()!='0':
            current=infeasible_outcome(case,b/name)
            if (b/name/'summary.json').exists() and json.loads((b/name/'summary.json').read_text())!=current:
                raise ValueError('infeasible outcome drift')
        else:
            recorded=json.loads((b/name/'summary.json').read_text())
            # analyze revalidates raw logs, event pairing and stage identities.
            current=analyze(case,b)
            if current!=recorded:raise ValueError('derived metric drift '+name)
            if suite=='phase':current['outcome']='complete'
        provenance=json.loads((b/'provenance.json').read_text())
        current['source_bundle']=str(b.resolve())
        current['simulator_sha256']=provenance['simulator_sha256']
        current['workload_sha256']=provenance['workloads'][suite]
        current['raw_log_sha256']=digest(b/name/'run.log')
        rows.append(current);sources[str(b.resolve())]=digest(b/'provenance.json')
    if len({r['simulator_sha256'] for r in rows})!=1 or len({r['workload_sha256'] for r in rows})!=1:
        raise ValueError('mixed simulator/workload versions')
    if output.exists():raise FileExistsError(output)
    output.parent.mkdir(parents=True,exist_ok=True)
    keys=sorted({k for r in rows for k in r})
    with output.open('w',newline='') as f:
        w=csv.DictWriter(f,fieldnames=keys);w.writeheader();w.writerows(sorted(rows,key=lambda r:r['name']))
    output.with_suffix('.manifest.json').write_text(json.dumps(dict(suite=suite,rows=len(rows),complete=sum(r.get('outcome','complete')=='complete' for r in rows),infeasible=sum(r.get('outcome','complete')!='complete' for r in rows),sources=sources,sha256=digest(output),statistical_unit='single deterministic configuration; no inferential confidence interval'),indent=2)+'\n')
    return rows

if __name__=='__main__':
    ap=argparse.ArgumentParser();ap.add_argument('--bundles',nargs='+',type=Path,required=True);ap.add_argument('--suite',choices=['read','write','phase'],required=True);ap.add_argument('--output',type=Path,required=True)
    args=ap.parse_args();print('Validated',len(collect(args.bundles,args.suite,args.output)),args.suite,'cases')
