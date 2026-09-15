#!/usr/bin/env python3
"""Matched placement/locality controls with explicit channel ownership evidence."""
from __future__ import annotations
import argparse
import concurrent.futures
import csv
import importlib.util
import json
import os
from pathlib import Path
import re
import shutil
import subprocess

ROOT = Path(__file__).resolve().parents[2]
HERE = Path(__file__).resolve().parent
spec = importlib.util.spec_from_file_location('boundary', ROOT/'experiments/17_system_boundaries/run.py')
boundary = importlib.util.module_from_spec(spec)
spec.loader.exec_module(boundary)


def matrix() -> list[dict]:
    return [dict(name=f'placement_t{tr}_{placement}_e{entries}', suite='read', pages=16,
                 entries=entries, depth=256, tr=tr, mshr=1, placement=placement)
            for tr in (20,12750) for placement in ('interleave','grouped','contiguous','balanced')
            for entries in (1,8,32)]


def configure(case: dict, out: Path) -> Path:
    path = boundary.configure(case,out)
    text = path.read_text()
    mode = {'interleave':0,'grouped':2,'contiguous':1,'balanced':2}[case['placement']]
    changes = {'gpgpu_hbf_placement_mode':mode,'gpgpu_hbf_channel_map':0,
               'gpgpu_hbf_channel_map_file':'""','gpgpu_hbf_tPROG':40 if case['tr']==20 else 170000,
               'gpgpu_hbf_tBERS':80 if case['tr']==20 else 1700000}
    if mode == 2:
        mapping = out/'configs'/(case['name']+'.map.csv')
        mapping.write_text(''.join(f'{p},{p//2%4 if case["placement"]=="grouped" else p//4}\n' for p in range(16)))
        changes['gpgpu_hbf_channel_map_file'] = f'"{mapping}"'
    for key,value in changes.items():
        pattern = r'^-'+re.escape(key)+r'\s+.*$'
        if len(re.findall(pattern,text,re.M)) != 1: raise ValueError(key)
        text = re.sub(pattern,lambda _: f'-{key} {value}',text,flags=re.M)
    path.write_text(text)
    return path


def main() -> None:
    ap=argparse.ArgumentParser();ap.add_argument('--tag',required=True);ap.add_argument('--jobs',type=int,default=2)
    args=ap.parse_args()
    if not re.fullmatch(r'[A-Za-z0-9][A-Za-z0-9._-]{0,79}',args.tag) or not 1<=args.jobs<=4: ap.error('invalid tag/jobs')
    out=HERE/'results'/args.tag;out.mkdir(parents=True,exist_ok=False)
    for d in ('configs','bin','source_snapshot'): (out/d).mkdir()
    cases=matrix();(out/'plan.json').write_text(json.dumps(cases,indent=2)+'\n')
    cuda=Path(os.environ.get('CUDA_INSTALL_PATH','/usr/local/cuda'));gsim=ROOT/'gpu-simulator/gpgpu-sim'
    source=ROOT/'experiments/13_rq2_locality/locality_probe.cu';binary=out/'bin/locality_probe'
    subprocess.run([str(cuda/'bin/nvcc'),'-arch=sm_70','--cudart','shared','-o',str(binary),str(source)],check=True)
    libs=list(gsim.glob('lib/gcc-*/cuda-*/release/libcudart.so'))
    if len(libs)!=1: raise ValueError('ambiguous simulator library')
    hashes={}
    for src in (ROOT/'hbf').glob('*'):
        if src.suffix not in ('.cc','.h'): continue
        if boundary.digest(src)!=boundary.digest(gsim/'src/gpgpu-sim'/src.name): raise ValueError('installed source drift')
        hashes[src.name]=boundary.digest(src);shutil.copy2(src,out/'source_snapshot'/src.name)
    for src in (source,Path(__file__),Path(boundary.__file__)):
        shutil.copy2(src,out/'source_snapshot'/(src.parent.name+'_'+src.name))
    prov=dict(simulator_sha256=boundary.digest(libs[0]),workload_sha256=boundary.digest(source),
              workload_binary_sha256=boundary.digest(binary),hbf_sources=hashes,
              actual_head=subprocess.check_output(['git','-C',str(gsim),'rev-parse','HEAD'],text=True).strip())
    (out/'provenance.json').write_text(json.dumps(prov,indent=2)+'\n')
    (out/'integration.diff').write_bytes(subprocess.check_output(['git','-C',str(gsim),'diff','HEAD','--binary']))
    env=dict(os.environ);env.update(CUDA_INSTALL_PATH=str(cuda),CUOBJDUMP_SIM_FILE='jj',REPO_ROOT=str(ROOT),GPGPUSIM_ROOT=str(gsim),
        ICXT_CFG=str(gsim/'configs/tested-cfgs/SM7_QV100/config_volta_islip.icnt'),LD_LIBRARY_PATH=str(libs[0].parent)+':'+env.get('LD_LIBRARY_PATH',''),RUN_TIMEOUT='900',HBF_DIAGNOSTICS='1')
    for c in cases: configure(c,out)
    def execute(c:dict)->dict:
        run=out/c['name']
        proc=subprocess.run(['bash',str(ROOT/'experiments/common/run_sim.sh'),str(run),str(out/'configs'/(c['name']+'.config')),str(binary),'16',str(c['entries']),str(boundary.BASE)],env=env,capture_output=True,text=True)
        if proc.returncode: raise RuntimeError(proc.stdout+proc.stderr)
        row=boundary.analyze(c,out)
        ingress=[r for r in csv.DictReader((run/'hbf.csv').open()) if r['state']=='INGRESS']
        observed={int(r['page']):int(r['channel']) for r in ingress}
        maps={'interleave':lambda p:p%4,'grouped':lambda p:p//2%4,'contiguous':lambda p:0,'balanced':lambda p:p//4}
        if observed!={p:maps[c['placement']](p) for p in range(16)}: raise ValueError('unexpected placement')
        if any(r['op']!='R' or int(r['bytes'])!=32 for r in ingress): raise ValueError('wrong operation/bytes')
        requested=sum(int(r['bytes']) for r in ingress)
        row.update(requested_read_bytes=requested,page_service_amplification=row['page_services']*4096/requested,
            array_read_amplification=row['array_reads']*4096/requested,
            channel_requests=json.dumps([sum(int(r['channel'])==i for r in ingress) for i in range(4)]),
            raw_log_sha256=boundary.digest(run/'run.log'),simulator_sha256=prov['simulator_sha256'],
            source_bundle=str(out),workload_sha256=prov['workload_sha256'])
        (run/'summary.json').write_text(json.dumps(row,indent=2)+'\n')
        print(c['name'],row['cycles'],'services/array',row['page_services'],row['array_reads'],flush=True)
        return row
    with concurrent.futures.ThreadPoolExecutor(max_workers=args.jobs) as pool:
        rows=list(pool.map(execute,cases))
    if boundary.digest(libs[0])!=prov['simulator_sha256']: raise ValueError('binary drift')
    with (out/'summary.csv').open('w',newline='') as f:
        w=csv.DictWriter(f,fieldnames=sorted(rows[0]));w.writeheader();w.writerows(rows)
    manifest=dict(rows=len(rows),sha256=boundary.digest(out/'summary.csv'),simulator_sha256=prov['simulator_sha256'],
                  provenance_sha256=boundary.digest(out/'provenance.json'),source_bundle=str(out),
                  statistical_unit='deterministic matched configuration; no inferential CI')
    (out/'summary.manifest.json').write_text(json.dumps(manifest,indent=2)+'\n')
    print('VALIDATED',len(rows),out,flush=True)

if __name__=='__main__': main()
