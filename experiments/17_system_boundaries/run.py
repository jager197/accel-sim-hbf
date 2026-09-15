#!/usr/bin/env python3
"""Predeclared paired experiments for admission and isolation boundaries."""
from __future__ import annotations
import argparse
import concurrent.futures
import csv
import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import subprocess
import sys

ROOT=Path(__file__).resolve().parents[2]
HERE=Path(__file__).resolve().parent
BASE=274877906944
sys.path.insert(0,str(ROOT/'util/traces'))
from validate_hbf_trace import validate


def digest(p:Path)->str:
    return hashlib.sha256(p.read_bytes()).hexdigest()


def matrix()->list[dict]:
    cases=[]
    # Base grid, and separate size robustness at the most informative extremes.
    for pages,entries_list,depths in [(16,(1,8,32),(32,128,4096)),(64,(8,32),(32,4096))]:
        for tr in (850,12750):
            for entries in entries_list:
                for depth in depths:
                    for mshr in (0,1):
                        name=f'read_p{pages}_e{entries}_q{depth}_t{tr}_m{mshr}'
                        cases.append(dict(name=name,suite='read',pages=pages,entries=entries,depth=depth,tr=tr,mshr=mshr))
    # Full timing: shared scheduling, and one/two/three reserved read channels.
    for wp in (1,4,16):
        for gap in (0,28300):
            for read_channels,scheduler in [(0,0),(0,1),(0,2),(1,1),(2,1),(3,1)]:
                name=f'write_w{wp}_g{gap}_r{read_channels}_s{scheduler}'
                cases.append(dict(name=name,suite='write',write_pages=wp,gap=gap,read_channels=read_channels,scheduler=scheduler,permutation=0,tr=12750))
    for rc in (0,1,2,3):
        cases.append(dict(name=f'alone_r{rc}',suite='write',write_pages=0,gap=0,read_channels=rc,scheduler=1,permutation=0,tr=12750))
    for wp in (4,16):
        for read_delay in (0,113200):
            for rc,sched in [(0,0),(0,1),(0,2),(1,1),(2,1),(3,1)]:
                cases.append(dict(name=f'phase_w{wp}_d{read_delay}_r{rc}_s{sched}',suite='phase',write_pages=wp,gap=0,read_channels=rc,scheduler=sched,permutation=0,tr=12750,read_delay=read_delay))
    for read_delay in (0,113200):
        cases.append(dict(name=f'phase_alone_d{read_delay}',suite='phase',write_pages=0,gap=0,read_channels=0,scheduler=1,permutation=0,tr=12750,read_delay=read_delay))
    # Confirm the predicted residual-program crossover with interior and
    # post-program arrival times; keep resource arms fixed before execution.
    for read_delay in (56600,169800,283000):
        for rc,sched in [(0,1),(3,1)]:
            cases.append(dict(name=f'phase_w4_d{read_delay}_r{rc}_s{sched}',suite='phase',write_pages=4,gap=0,read_channels=rc,scheduler=sched,permutation=0,tr=12750,read_delay=read_delay))
        cases.append(dict(name=f'phase_alone_d{read_delay}',suite='phase',write_pages=0,gap=0,read_channels=0,scheduler=1,permutation=0,tr=12750,read_delay=read_delay))
    for rc in (0,3):
        cases.append(dict(name=f'phase_w16_d113200_r{rc}_s1_deadline340000',suite='phase',write_pages=16,gap=0,read_channels=rc,scheduler=1,permutation=0,tr=12750,read_delay=113200,assembly_deadline=340000))
    return cases


def configure(case:dict,out:Path,diagnostics:bool=True)->Path:
    suite=case['suite']
    template=ROOT/'artifact/configs'/('reads.config' if suite=='read' else 'writes.config')
    text=template.read_text()
    change={'gpgpu_hbf_tR':case['tr'], 'gpgpu_hbf_trace_file':f'"{out/case["name"]/"hbf.csv"}"'}
    if suite=='read':
        change.update(gpgpu_hbf_max_outstanding=case['depth'],gpgpu_hbf_mshr_enabled=case['mshr'])
    else:
        # Large bound avoids class-dependent admission exclusion; experiment
        # isolates media sharing, not an undersized aggregation buffer.
        change.update(gpgpu_hbf_max_outstanding=4096,gpgpu_hbf_scheduler=case['scheduler'],
                      gpgpu_hbf_write_buffer_entries=256,gpgpu_hbf_write_agg_timeout=case.get('assembly_deadline',10000))
        rc=case['read_channels'];wp=case['write_pages']
        pairs=[(p,p%(rc or 4)) for p in range(128)]
        pairs += [(262144+p, rc+p%(4-rc) if rc else p%4) for p in range(wp)]
        mapping=out/'configs'/(case['name']+'.map.csv')
        mapping.write_text(''.join(f'{p},{c}\n' for p,c in pairs))
        change['gpgpu_hbf_channel_map_file']=f'"{mapping}"'
    for key,value in change.items():
        pattern=r'^-'+key+r'\s+.*$'
        if len(re.findall(pattern,text,re.M))!=1:raise ValueError(key)
        text=re.sub(pattern,lambda _:f'-{key} {value}',text,flags=re.M)
    p=out/'configs'/(case['name']+'.config');p.write_text(text);return p


def quantile(values:list[int],pct:int)->int:
    values=sorted(values)
    return values[(len(values)-1)*pct//100] if values else 0


def analyze(case:dict,out:Path,diagnostics:bool=True)->dict:
    path=out/case['name']
    expected=case['pages']*case['entries']*4 if case['suite']=='read' else 512+case['write_pages']*128
    rep=validate(path/'hbf.csv',4,32 if case['suite']=='read' else 4,expected,False,True)
    (path/'trace_validation.json').write_text(json.dumps(rep,indent=2)+'\n')
    if not rep['valid']:raise ValueError(rep['errors'])
    if (path/'run.rc').read_text().strip()!='0':raise ValueError('run failed')
    log=(path/'run.log').read_text(errors='replace')
    if ('[LOCALITY-PROBE]' if case['suite']=='read' else '[MIXED-BOUNDARY]') not in log:raise ValueError('completion missing')
    total_cycles=int(re.findall(r'^gpu_sim_cycle =\s*(\d+)',log,re.M)[-1])
    if total_cycles>=2**32:raise ValueError('creation timestamp wrap')
    log=log[log.rfind('========= HBF System Statistics ========='):]
    def stat(label:str)->int:
        v=re.findall('^'+re.escape(label)+r'\s*(\d+)',log,re.M)
        if len(v)!=1:raise ValueError(f'missing/duplicate {label}')
        return int(v[0])
    for l in ['HBF Incomplete Page Errors:','HBF Write Coverage Errors:','HBF Mapping Errors:','HBF Padding Bytes:']:
        if stat(l):raise ValueError(l)
    result=dict(case,cycles=total_cycles,trace_valid=1,run_rc=0)
    result.update(page_services=stat('HBF Page Reads:'),buffer_hits=stat('HBF Page Buffer Hits:'),
                  programs=stat('HBF Page Programs:'),write_bytes=stat('HBF Host Write Bytes:'),
                  mshr_hits=stat('HBF MSHR Hits:'))
    result['array_reads']=result['page_services']-result['buffer_hits']
    if case['suite']!='read' and (result['programs']!=case['write_pages'] or result['write_bytes']!=case['write_pages']*4096):raise ValueError('write conservation')
    rows=list(csv.DictReader((path/'hbf.csv').open()))
    ingress={r['request_id']:r for r in rows if r['state']=='INGRESS'}
    done={r['request_id']:r for r in rows if r['state']=='COMPLETED'}
    if any(r['error'] for r in rows):raise ValueError('error record')
    drows=list(csv.DictReader((path/'hbf.csv.diagnostics.csv').open())) if diagnostics else []
    di={r['request_id']:r for r in drows if r['state']=='INGRESS'}
    dd={r['request_id']:r for r in drows if r['state']=='COMPLETED'}
    if diagnostics and (set(di)!=set(ingress) or set(dd)!=set(done)):raise ValueError('diagnostic conservation')
    phases=[]
    for op,label in [('R','read'),('W','write')]:
        ctrl=[];up=[];end=[]
        for key,r in done.items():
            if r['op']!=op:continue
            start=int(ingress[key]['sim_cycle']);finish=int(r['sim_cycle']);ctrl.append(finish-start)
            if diagnostics:
                created=int(di[key]['created_cycle'])
                if not created<=start<=finish:raise ValueError('timestamp ordering')
                if di[key]['mem_fetch_uid']!=dd[key]['mem_fetch_uid'] or created!=int(dd[key]['created_cycle']):raise ValueError('identity drift')
                up.append(start-created);end.append(finish-created)
                phases.append(dict(request_id=key,op=op,created=created,admitted=start,returned=finish,
                                   upstream=start-created,controller=finish-start,total=finish-created,channel=r['channel']))
        result[label+'_count']=len(ctrl)
        for metric,vals in [('controller',ctrl),('upstream',up),('total',end)]:
            for q in (50,95,99):result[f'{label}_{metric}_p{q}']=quantile(vals,q)
            result[f'{label}_{metric}_mean']=sum(vals)/len(vals) if vals else 0
    result['ingress_span']=max(int(r['sim_cycle']) for r in ingress.values())-min(int(r['sim_cycle']) for r in ingress.values())
    if diagnostics:
        result['read_queue_request_ticks']=stat('HBF Diagnostic Read Queue Request Ticks:')
        result['read_program_request_ticks']=stat('HBF Diagnostic Read PROGRAM Request Ticks:')
        if result['read_program_request_ticks']>result['read_queue_request_ticks']:raise ValueError('wait subset')
        with (path/'phases.csv').open('w',newline='') as f:
            w=csv.DictWriter(f,fieldnames=list(phases[0]));w.writeheader();w.writerows(phases)
    result['trace_sha256']=digest(path/'hbf.csv');result['config_sha256']=digest(path/'gpgpusim.config')
    if diagnostics:result['diagnostic_sha256']=digest(path/'hbf.csv.diagnostics.csv')
    return result


def infeasible_outcome(case:dict,path:Path)->dict:
    """Retain the one diagnosed deadline failure, never substitute performance."""
    if case['name']!='phase_w16_d113200_r3_s1':
        raise ValueError('undeclared infeasibility')
    log=(path/'run.log').read_text(errors='replace')
    if (path/'run.rc').read_text().strip()!='1' or 'page=262159: strict write policy rejected an incomplete NAND page' not in log:
        raise ValueError('unexpected failure cause')
    with (path/'hbf.csv').open() as stream:
        events=list(csv.DictReader(stream))
    fragments=[e for e in events if e['op']=='W' and e['page']=='262159' and e['state']=='INGRESS']
    if (len(fragments)!=125 or len({e['request_id'] for e in fragments})!=125
            or len({e['address'] for e in fragments})!=125
            or any(int(e['bytes'])!=32 for e in fragments)
            or any(e['state']=='PROGRAM' and e['page']=='262159' for e in events)):
        raise ValueError('unexpected incomplete-page pattern')
    return dict(case,outcome='infeasible_assembly_deadline',run_rc=1,trace_valid=0,
                admitted_last_page_sectors=125,expected_last_page_sectors=128,
                trace_sha256=digest(path/'hbf.csv'),config_sha256=digest(path/'gpgpusim.config'))


def main()->None:
    ap=argparse.ArgumentParser();ap.add_argument('--tag',required=True);ap.add_argument('--pilot',action='store_true');ap.add_argument('--jobs',type=int,default=4);ap.add_argument('--no-diagnostics',action='store_true');ap.add_argument('--cases',nargs='+')
    args=ap.parse_args()
    if not re.fullmatch(r'[A-Za-z0-9][A-Za-z0-9._-]{0,79}',args.tag) or not 1<=args.jobs<=4:ap.error('invalid tag/jobs')
    out=HERE/'results'/args.tag;out.mkdir(parents=True,exist_ok=False);(out/'configs').mkdir();(out/'bin').mkdir();(out/'source_snapshot').mkdir()
    cases=matrix()
    if not args.cases and not args.pilot:
        cases=[c for c in cases if c['suite'] in ('read','phase')]
    if args.pilot:
        names={'read_p16_e32_q128_t12750_m0','read_p16_e32_q128_t12750_m1',
               'write_w4_g0_r0_s0','write_w4_g28300_r0_s0','write_w16_g28300_r2_s1','alone_r0'}
        cases=[c for c in cases if c['name'] in names]
    if args.cases:
        cases=[c for c in cases if c['name'] in args.cases]
        if {c['name'] for c in cases}!=set(args.cases):raise ValueError('unknown case selection')
    (out/'plan.json').write_text(json.dumps(cases,indent=2)+'\n')
    cuda=Path(os.environ.get('CUDA_INSTALL_PATH','/usr/local/cuda'));gsim=ROOT/'gpu-simulator/gpgpu-sim'
    sources={'read':ROOT/'experiments/13_rq2_locality/locality_probe.cu','write':HERE/'mixed_boundary.cu','phase':HERE/'mixed_boundary.cu'}
    for suite,src in sources.items():
        subprocess.run([str(cuda/'bin/nvcc'),'-arch=sm_70','--cudart','shared','-o',str(out/'bin'/suite),str(src)],check=True)
        shutil.copy2(src,out/'source_snapshot'/src.name)
    for src in (Path(__file__),ROOT/'artifact/configs/reads.config',ROOT/'artifact/configs/writes.config'):
        shutil.copy2(src,out/'source_snapshot'/(src.parent.name+'_'+src.name))
    libs=list(gsim.glob('lib/gcc-*/cuda-*/release/libcudart.so'))
    if len(libs)!=1:raise ValueError('ambiguous library')
    hashes={}
    for src in (ROOT/'hbf').glob('*'):
        if src.suffix not in ('.cc','.h'):continue
        installed=gsim/'src/gpgpu-sim'/src.name
        if digest(src)!=digest(installed):raise ValueError('installed source drift')
        hashes[src.name]=digest(src);shutil.copy2(src,out/'source_snapshot'/src.name)
    prov=dict(simulator_sha256=digest(libs[0]),hbf_sources=hashes,
              actual_head=subprocess.check_output(['git','-C',str(gsim),'rev-parse','HEAD'],text=True).strip(),
              diagnostics=not args.no_diagnostics,workloads={k:digest(v) for k,v in sources.items()})
    (out/'provenance.json').write_text(json.dumps(prov,indent=2)+'\n')
    (out/'integration.diff').write_bytes(subprocess.check_output(['git','-C',str(gsim),'diff','HEAD','--binary']))
    env=dict(os.environ);env.update(CUDA_INSTALL_PATH=str(cuda),CUOBJDUMP_SIM_FILE='jj',REPO_ROOT=str(ROOT),GPGPUSIM_ROOT=str(gsim),
              ICXT_CFG=str(gsim/'configs/tested-cfgs/SM7_QV100/config_volta_islip.icnt'),LD_LIBRARY_PATH=str(libs[0].parent)+':'+env.get('LD_LIBRARY_PATH',''),RUN_TIMEOUT='900',HBF_DIAGNOSTICS='0' if args.no_diagnostics else '1')
    for c in cases:configure(c,out)
    def execute(c:dict)->dict:
        argv=[str(c['pages']),str(c['entries']),str(BASE)] if c['suite']=='read' else [str(c['write_pages']),str(c['gap']),str(c['permutation']),str(BASE),str(c.get('read_delay',0))]
        proc=subprocess.run(['bash',str(ROOT/'experiments/common/run_sim.sh'),str(out/c['name']),str(out/'configs'/(c['name']+'.config')),str(out/'bin'/c['suite']),*argv],env=env,capture_output=True,text=True)
        if proc.returncode:
            if c['name']!='phase_w16_d113200_r3_s1':raise RuntimeError(proc.stdout+proc.stderr)
            result=infeasible_outcome(c,out/c['name'])
            (out/c['name']/'summary.json').write_text(json.dumps(result,indent=2)+'\n')
            print(c['name'],'INFEASIBLE: assembly deadline',flush=True)
            return result
        result=analyze(c,out,not args.no_diagnostics)
        (out/c['name']/'summary.json').write_text(json.dumps(result,indent=2)+'\n')
        print(c['name'],result['cycles'],result['read_controller_p95'],result['read_total_p95'],flush=True)
        return result
    rows=[]
    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as pool:
        for f in concurrent.futures.as_completed([pool.submit(execute,c) for c in cases]):rows.append(f.result())
    if digest(libs[0])!=prov['simulator_sha256'] or len(rows)!=len(cases):raise ValueError('incomplete/drift')
    rows.sort(key=lambda r:r['name']);keys=sorted({k for r in rows for k in r})
    with (out/'summary.csv').open('w',newline='') as f:
        w=csv.DictWriter(f,fieldnames=keys);w.writeheader();w.writerows(rows)
    print('VALIDATED',len(rows),out,flush=True)

if __name__=='__main__':main()
