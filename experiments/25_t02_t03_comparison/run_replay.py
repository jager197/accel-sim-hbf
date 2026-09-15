#!/usr/bin/env python3
"""Same-controller identity replay and frozen-ingress counterfactuals."""
from __future__ import annotations
import csv, hashlib, json, os, re, subprocess
from collections import Counter
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
SOURCE=Path(__file__).resolve().parent
TAG=os.environ.get('RUN_TAG','comparison-001')
if not re.fullmatch(r'[A-Za-z0-9][A-Za-z0-9._-]{0,79}',TAG):raise ValueError('invalid RUN_TAG')
HERE=SOURCE/'results'/TAG
FULL=ROOT/'experiments/17_system_boundaries/results'/TAG
def digest(p:Path)->str:return hashlib.sha256(p.read_bytes()).hexdigest()
def events(p:Path)->list[dict[str,str]]:
    with p.open() as f:return list(csv.DictReader(f))
def ingress(rows:list[dict[str,str]])->list[dict[str,str]]:return [r for r in rows if r['state']=='INGRESS']
def measured(rows:list[dict[str,str]])->dict:
    start={r['request_id']:r for r in ingress(rows)};done={r['request_id']:r for r in rows if r['state']=='COMPLETED'}
    assert len(start)==len(done) and start.keys()==done.keys()
    assert not any(r['error'] for r in rows)
    assert all((start[k]['address'],start[k]['bytes'])==(r['address'],r['bytes']) for k,r in done.items())
    lo=min(int(r['sim_cycle']) for r in start.values());hi=max(int(r['sim_cycle']) for r in done.values())
    lat=sorted(int(r['sim_cycle'])-int(start[k]['sim_cycle']) for k,r in done.items())
    return dict(requests=len(start),first=lo,last=hi,span=hi-lo,ingress_span=max(int(r['sim_cycle']) for r in start.values())-lo,controller_p95=lat[(len(lat)-1)*95//100],controller_mean=sum(lat)/len(lat),page_services=sum(r['state']=='READ' for r in rows),bytes=sum(int(r['bytes']) for r in start.values()))
def execute(name:str,source:Path,target:Path)->tuple[list[dict[str,str]],Path]:
    out=HERE/'replays'/name;out.mkdir(parents=True,exist_ok=False)
    raw=events(source/'hbf.csv'); ins=ingress(raw)
    (out/'input.tsv').write_text(''.join(f"{r['sim_cycle']} {r['address']} {r['bytes']} {r['source_subpartition']}\n" for r in ins))
    config=(target/'gpgpusim.config').read_text();config=re.sub(r'^-gpgpu_hbf_trace_file .*$',f'-gpgpu_hbf_trace_file "{out}/hbf.csv"',config,flags=re.M)
    (out/'gpgpusim.config').write_text(config)
    (out/'config_volta_islip.icnt').symlink_to(ROOT/'gpu-simulator/gpgpu-sim/configs/tested-cfgs/SM7_QV100/config_volta_islip.icnt')
    env=dict(os.environ,HBF_DIAGNOSTICS='1')
    with (out/'run.log').open('w') as log:subprocess.run([str(SOURCE/'replay'),str(out/'input.tsv')],cwd=out,env=env,stdout=log,stderr=subprocess.STDOUT,check=True,timeout=120)
    (out/'provenance.json').write_text(json.dumps(dict(source=str(source),target=str(target),source_trace_sha256=digest(source/'hbf.csv'),target_config_sha256=digest(target/'gpgpusim.config'),input_sha256=digest(out/'input.tsv'),binary_sha256=digest(SOURCE/'replay')),indent=2)+'\n')
    return events(out/'hbf.csv'),out

def main()->None:
    cases=json.loads((FULL/'plan.json').read_text());checks=[];results=[]
    for c in cases:
        name=c['name'];raw=events(FULL/name/'hbf.csv');rep,out=execute('self_'+name,FULL/name,FULL/name)
        # All published controller event fields must reproduce, not only totals.
        equal=raw==rep
        checks.append(dict(name=name,exact=equal,events=len(raw),full=measured(raw),replay=measured(rep)))
        if not equal:
            (HERE/'self_validation.json').write_text(json.dumps(checks,indent=2));raise ValueError('identity replay mismatch: '+name)
        donor=name[:-1]+str(1-c['mshr']);target=measured(raw)
        rr,ro=execute('cross_'+name,FULL/donor,FULL/name);m=measured(rr)
        a=Counter((r['address'],r['bytes'],r['op']) for r in ingress(raw));b=Counter((r['address'],r['bytes'],r['op']) for r in ingress(rr));assert a==b
        results.append(dict(c,donor=donor,full_span=target['span'],replay_span=m['last']-target['first'],gap_pct=100*((m['last']-target['first'])/target['span']-1),full_ingress_span=target['ingress_span'],replay_ingress_span=m['ingress_span'],full_p95=target['controller_p95'],replay_p95=m['controller_p95'],full_services=target['page_services'],replay_services=m['page_services'],requests=m['requests'],trace_sha256=digest(ro/'hbf.csv')))
        print(name,'gap',round(results[-1]['gap_pct'],2),flush=True)
    (HERE/'self_validation.json').write_text(json.dumps(checks,indent=2)+'\n')
    with (HERE/'replay_summary.csv').open('w',newline='') as f:w=csv.DictWriter(f,fieldnames=list(results[0]));w.writeheader();w.writerows(results)
if __name__=='__main__':main()
