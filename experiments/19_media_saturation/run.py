#!/usr/bin/env python3
"""Default-timing C++ media-saturation study with measured occupancy checks."""
from __future__ import annotations
import argparse
from collections import Counter
import concurrent.futures
import csv
import hashlib
import json
import math
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys
ROOT=Path(__file__).resolve().parents[2]
HERE=Path(__file__).resolve().parent
sys.path.insert(0,str(ROOT/'util/traces'))
from validate_hbf_trace import validate
BASE=274877906944


def sha(path:Path)->str:return hashlib.sha256(path.read_bytes()).hexdigest()
def options(path:Path)->dict[str,str]:return dict(re.findall(r'^-(\S+)\s+([^\n]+)',path.read_text(),re.M))


def matrix()->list[dict]:
    cells=[dict(name=f'grow_c{c}_p1024_n1024',channels=c,pages=1024,producers=1024,subarrays=32*c,active=32*c,group='grow') for c in [1,2,4,8,16]]
    cells += [dict(name=f'supply_c16_p1024_n{n}',channels=16,pages=1024,producers=n,subarrays=512,active=512,group='supply') for n in [256,512]]
    cells += [dict(name='size_c16_p2048_n2048',channels=16,pages=2048,producers=2048,subarrays=512,active=512,group='size'),
              dict(name='fixed_c16_p1024_n1024',channels=16,pages=1024,producers=1024,subarrays=32,active=32,group='fixed')]
    return cells


def configure(case:dict,out:Path)->Path:
    template=ROOT/'artifact/configs/scaling.config'
    text=template.read_text()
    changes=dict(gpgpu_hbf_num_channels=case['channels'],gpgpu_hbf_num_subarrays=case['subarrays'],gpgpu_hbf_max_active=case['active'],
                 gpgpu_hbf_max_outstanding=4096,gpgpu_hbf_cache_entries=0,gpgpu_hbf_buffer_enabled=0,
                 gpgpu_hbf_trace_file=f'"{out/case["name"]/"hbf.csv"}"')
    for key,value in changes.items():
        pattern=r'^-'+key+r'\s+.*$'
        if len(re.findall(pattern,text,re.M))!=1:raise ValueError('missing/duplicate '+key)
        text=re.sub(pattern,lambda _:f'-{key} {value}',text,flags=re.M)
    p=out/'configs'/(case['name']+'.config');p.write_text(text);return p


def analyze(case:dict,out:Path)->dict:
    path=out/case['name'];cfg=options(path/'gpgpusim.config')
    n=case['pages'];channels=case['channels'];subarrays=case['subarrays'];slots=min(subarrays,case['active'])
    if (path/'run.rc').read_text().strip()!='0':raise ValueError('failed execution')
    rep=validate(path/'hbf.csv',channels,subarrays,n,False,True)
    if not rep['valid']:raise ValueError(rep['errors'])
    (path/'trace_validation.json').write_text(json.dumps(rep,indent=2)+'\n')
    events=list(csv.DictReader((path/'hbf.csv').open()))
    ins={e['request_id']:e for e in events if e['state']=='INGRESS'}
    done={e['request_id']:e for e in events if e['state']=='COMPLETED'}
    reads=[e for e in events if e['state']=='READ']
    if set(ins)!=set(done) or len(reads)!=n:raise ValueError('conservation')
    if {int(e['page']) for e in ins.values()}!=set(range(n)):raise ValueError('page set')
    if any(e['op']!='R' or int(e['bytes'])!=32 or int(e['address'])!=BASE+int(e['page'])*4096 for e in ins.values()):raise ValueError('unexpected request')
    if Counter(int(e['channel']) for e in reads)!={c:n//channels for c in range(channels)}:raise ValueError('channel balance')
    if len({e['page'] for e in reads})!=n:raise ValueError('repeat array services')
    if {int(e['subarray']) for e in reads}!=set(range(subarrays)):raise ValueError('idle physical resources')
    clocks=list(map(float,cfg['gpgpu_clock_domains'].split(':')));core,dram=clocks[0],clocks[3];tr=int(cfg['gpgpu_hbf_tR'])
    if cfg['gpgpu_hbf_cache_entries']!='0' or cfg['gpgpu_hbf_buffer_enabled']!='0' or tr!=12750:raise ValueError('cache/timing drift')
    duration=tr*core/dram;busy_until={};edges=[]
    for r in reads:
        cycle=int(r['sim_cycle']);sa=int(r['subarray']);ch=int(r['channel']);per=subarrays//channels
        if not ch*per<=sa<(ch+1)*per:raise ValueError('affinity')
        if cycle < busy_until.get(sa,0)-1:raise ValueError('overlap')
        busy_until[sa]=cycle+duration;edges.extend([(cycle,1),(cycle+duration,-1)])
        i=ins[r['request_id']];d=done[r['request_id']]
        if int(d['sim_cycle'])<cycle+duration-1 or int(i['sim_cycle'])>cycle:raise ValueError('read timing')
    count=peak=0
    for t,delta in sorted(edges):count+=delta;peak=max(peak,count)
    if peak>slots:raise ValueError('exceeded active slots')
    first=min(int(r['sim_cycle']) for r in ins.values());end=max(int(r['sim_cycle']) for r in done.values());span=end-first
    media_first=min(int(r['sim_cycle']) for r in reads);media_last=max(int(r['sim_cycle']) for r in reads)+duration
    log=(path/'run.log').read_text(errors='replace')
    if f'[PAGE-SUPPLY] completed: {n} pages, {case["producers"]} producers' not in log:raise ValueError('missing completion')
    for label in ['HBF Mapping Errors:','HBF Incomplete Page Errors:','HBF Page Buffer Hits:']:
        if int(re.findall(r'^'+re.escape(label)+r'\s*(\d+)',log,re.M)[-1])!=0:raise ValueError(label)
    observed=int(re.findall(r'^HBF Page Reads:\s*(\d+)',log,re.M)[-1])
    if observed!=n:raise ValueError('array accounting')
    cycles=int(re.findall(r'^gpu_sim_cycle =\s*(\d+)',log,re.M)[-1])
    if cycles>=2**32:raise ValueError('timestamp wrap')
    requested=n*32;arraybytes=n*4096
    result=dict(case,cycles=cycles,span_core_cycles=span,ingress_span_core_cycles=max(int(e['sim_cycle']) for e in ins.values())-first,
        first_wave_issued=sum(int(r['sim_cycle'])<media_first+duration for r in reads),peak_active=peak,
        media_utilization=n*duration/(slots*(media_last-media_first)),service_efficiency=n*duration/(slots*span),
        requested_bytes=requested,array_bytes=arraybytes,array_reads=n,
        array_bandwidth_gbs=arraybytes*core/1000/span,returned_bandwidth_gbs=requested*core/1000/span,
        useful_lane_bandwidth_gbs=n*4*core/1000/span,array_ceiling_gbs=slots*4096*dram/1000/tr,
        trace_sha256=sha(path/'hbf.csv'),config_sha256=sha(path/'gpgpusim.config'),raw_log_sha256=sha(path/'run.log'),trace_valid=1,run_rc=0)
    return result


def main()->None:
    ap=argparse.ArgumentParser();ap.add_argument('--tag',required=True);ap.add_argument('--jobs',type=int,default=4);ap.add_argument('--cases',nargs='+')
    args=ap.parse_args()
    if not re.fullmatch(r'[A-Za-z0-9][A-Za-z0-9._-]{0,79}',args.tag) or not 1<=args.jobs<=4:ap.error('invalid tag/jobs')
    cells=matrix()
    if args.cases:
        cells=[c for c in cells if c['name'] in args.cases]
        if {c['name'] for c in cells}!=set(args.cases):raise ValueError('unknown cells')
    out=HERE/'results'/args.tag;out.mkdir(parents=True,exist_ok=False)
    for d in ['configs','bin','snapshot']:(out/d).mkdir()
    (out/'plan.json').write_text(json.dumps(cells,indent=2)+'\n')
    cuda=Path(os.environ.get('CUDA_INSTALL_PATH','/usr/local/cuda'));gsim=ROOT/'gpu-simulator/gpgpu-sim';libs=list(gsim.glob('lib/gcc-*/cuda-*/release/libcudart.so'))
    if len(libs)!=1:raise ValueError('ambiguous library')
    source=HERE/'page_supply.cu';binary=out/'bin/page_supply'
    subprocess.run([str(cuda/'bin/nvcc'),'-arch=sm_70','--cudart','shared','-o',str(binary),str(source)],check=True)
    prov=dict(simulator_sha256=sha(libs[0]),source_sha256=sha(source),binary_sha256=sha(binary),hbf_sources={})
    for p in (ROOT/'hbf').glob('*'):
        if p.suffix not in ['.cc','.h']:continue
        if sha(p)!=sha(gsim/'src/gpgpu-sim'/p.name):raise ValueError('installed drift')
        prov['hbf_sources'][p.name]=sha(p);shutil.copy2(p,out/'snapshot'/p.name)
    for p in [Path(__file__),source]:shutil.copy2(p,out/'snapshot'/p.name)
    (out/'provenance.json').write_text(json.dumps(prov,indent=2)+'\n');(out/'integration.diff').write_bytes(subprocess.check_output(['git','-C',str(gsim),'diff','HEAD','--binary']))
    env=dict(os.environ);env.update(CUDA_INSTALL_PATH=str(cuda),CUOBJDUMP_SIM_FILE='jj',REPO_ROOT=str(ROOT),GPGPUSIM_ROOT=str(gsim),ICXT_CFG=str(gsim/'configs/tested-cfgs/SM7_QV100/config_volta_islip.icnt'),LD_LIBRARY_PATH=str(libs[0].parent)+':'+env.get('LD_LIBRARY_PATH',''),RUN_TIMEOUT='1200',HBF_DIAGNOSTICS='0')
    for c in cells:configure(c,out)
    def execute(c:dict)->dict:
        cmd=['bash',str(ROOT/'experiments/common/run_sim.sh'),str(out/c['name']),str(out/'configs'/(c['name']+'.config')),str(binary),str(BASE),str(c['pages']),str(c['producers'])]
        p=subprocess.run(cmd,env=env,capture_output=True,text=True)
        if p.returncode:raise RuntimeError(p.stdout+p.stderr)
        result=analyze(c,out);result.update(source_bundle=str(out),simulator_sha256=prov['simulator_sha256'],workload_sha256=prov['source_sha256'])
        (out/c['name']/'summary.json').write_text(json.dumps(result,indent=2)+'\n')
        print(c['name'],f'array={result["array_bandwidth_gbs"]:.3f} GB/s returned={result["returned_bandwidth_gbs"]:.3f} peak={result["peak_active"]}/{c["active"]} utilization={result["media_utilization"]:.5f}',flush=True)
        return result
    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as pool:results=list(pool.map(execute,cells))
    if sha(libs[0])!=prov['simulator_sha256']:raise ValueError('binary drift')
    with (out/'summary.csv').open('w',newline='') as f:
        writer=csv.DictWriter(f,fieldnames=list(results[0]));writer.writeheader();writer.writerows(results)
    (out/'summary.manifest.json').write_text(json.dumps(dict(rows=len(results),sha256=sha(out/'summary.csv'),source_bundle=str(out),simulator_sha256=prov['simulator_sha256'],definitions='array bytes count 4096 per READ; returned bytes count 32 per lane; useful lane load is 4 B; utilization derived from READ issue times and configured tR'),indent=2)+'\n')
    print('VALIDATED',len(results),out,flush=True)

if __name__=='__main__':main()
