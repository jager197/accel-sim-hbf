#!/usr/bin/env python3
"""Run the curated manuscript experiments without manuscript or private run files."""
from __future__ import annotations
import argparse
import datetime as dt
import importlib.util
import json
import os
from pathlib import Path
import re
import subprocess
import sys

ROOT=Path(__file__).resolve().parents[1]
CORE=('validation','trace','comparison','media','placement','hotspots','isolation','cost')
SUITES=CORE+('qwen',)

def commands(suite: str, tag: str, jobs: int) -> list[list[str]]:
    py=sys.executable
    exp='experiments/'
    if suite=='validation':return [['bash',exp+'10_validation/run.sh']]
    if suite=='trace':return [['bash',exp+'11_trace_smoke/run.sh']]
    if suite=='cost':return [['bash',exp+'15_cost_capacity/run.sh']]
    if suite=='comparison':
        d=exp+'25_t02_t03_comparison/'
        return [['bash',d+'build.sh']]+[[py,d+name+'.py'] for name in ('run_full','run_replay','run_external','analyze')]
    if suite in ('media','placement','hotspots'):
        directory={'media':'19_media_saturation','placement':'18_placement_cases','hotspots':'20_placement_hotspots'}[suite]
        return [[py,exp+directory+'/run.py','--tag',tag,'--jobs',str(jobs)]]
    if suite=='isolation':
        d=exp+'17_system_boundaries/'
        spec=importlib.util.spec_from_file_location('boundary',ROOT/d/'run.py')
        module=importlib.util.module_from_spec(spec);spec.loader.exec_module(module)
        names=[c['name'] for c in module.matrix() if c['suite']=='phase']
        # The comparison and phase studies share the runner but use distinct tags.
        return [[py,d+'run.py','--tag',tag+'-phase','--jobs',str(jobs),'--cases',*names],
                [py,d+'collect.py','--bundles',d+'results/'+tag+'-phase','--suite','phase','--output',d+'results/'+tag+'-phase/phase_boundary_summary.csv']]
    if suite=='qwen':
        d=exp+'23_qwen3_1p7b/'
        traces=os.environ.get('QWEN_TRACE_DIR',str(ROOT/'artifact_runs/inputs/qwen3'))
        out=d+'results/'+tag
        return [[py,d+'run_smoke.py','--trace-dir',traces,'--output',out],
                [py,d+'validate_replay.py','--run',out+'/run','--traces',traces,'--output',out+'/summary.json']]
    raise ValueError(suite)

def expected_output(suite: str, tag: str) -> Path:
    paths={
        'validation':f'experiments/10_validation/results/{tag}/channel_summary.csv',
        'trace':f'experiments/11_trace_smoke/results/{tag}/run/trace_validation.json',
        'comparison':f'experiments/25_t02_t03_comparison/results/{tag}/validation.json',
        'media':f'experiments/19_media_saturation/results/{tag}/summary.csv',
        'placement':f'experiments/18_placement_cases/results/{tag}/summary.csv',
        'hotspots':f'experiments/20_placement_hotspots/results/{tag}/summary.csv',
        'isolation':f'experiments/17_system_boundaries/results/{tag}-phase/phase_boundary_summary.csv',
        'cost':f'experiments/15_cost_capacity/results/{tag}/summary.json',
        'qwen':f'experiments/23_qwen3_1p7b/results/{tag}/summary.json',
    }
    return ROOT/paths[suite]

def main() -> int:
    ap=argparse.ArgumentParser(description=__doc__)
    ap.add_argument('--suite',choices=('core','all',*SUITES),default='core')
    mode=ap.add_mutually_exclusive_group(required=True);mode.add_argument('--plan',action='store_true');mode.add_argument('--run',action='store_true')
    ap.add_argument('--tag',default=dt.datetime.now(dt.timezone.utc).strftime('%Y%m%d-%H%M%S'))
    ap.add_argument('--jobs',type=int,default=1);a=ap.parse_args()
    if not re.fullmatch(r'[A-Za-z0-9][A-Za-z0-9._-]{0,69}',a.tag) or not 1<=a.jobs<=4:ap.error('use a fresh tag (max 70 characters) and 1..4 jobs')
    suites=SUITES if a.suite=='all' else CORE if a.suite=='core' else (a.suite,)
    plan={s:commands(s,a.tag,a.jobs) for s in suites}
    if a.plan:
        print(json.dumps({'suites':plan,'dependencies':{'comparison':'make setup-mqsim','qwen':'make prepare-qwen (or QWEN_TRACE_DIR)'},'core_excludes':'qwen (long full-model run)'},indent=2));return 0
    subprocess.run(['bash',str(ROOT/'scripts/verify_install.sh')],check=True)
    if 'comparison' in suites:
        mq=ROOT/'experiments/25_t02_t03_comparison/vendor/MQSim'
        if not (mq/'MQSim').is_file():ap.error('run make setup-mqsim first')
        rev=subprocess.check_output(['git','-C',str(mq),'rev-parse','HEAD'],text=True).strip()
        if rev!='51f0f2d3fed92d88ef4a0fa61a38024b07bf9d16':ap.error('MQSim revision mismatch')
        subprocess.run(['git','-C',str(mq),'diff','--exit-code','HEAD'],check=True,stdout=subprocess.DEVNULL)
    if 'qwen' in suites:
        traces=Path(os.environ.get('QWEN_TRACE_DIR',str(ROOT/'artifact_runs/inputs/qwen3')))
        if not (traces/'kernelslist.g').is_file():ap.error('run make prepare-qwen or set QWEN_TRACE_DIR first')
    for s in suites:
        if expected_output(s,a.tag).parent.exists():ap.error(f'output already exists for {s}: choose a new tag')
    out=ROOT/'artifact_runs/reproduction'/a.tag;out.mkdir(parents=True,exist_ok=False)
    manifest={'tag':a.tag,'status':'running','suites':[]}
    env=dict(os.environ,RUN_TAG=a.tag,MAX_PAR=str(a.jobs),PYTHON=sys.executable)
    for suite,steps in plan.items():
        record={'suite':suite,'commands':steps,'status':'running'};manifest['suites'].append(record)
        (out/'manifest.json').write_text(json.dumps(manifest,indent=2)+'\n')
        print('Running',suite,flush=True)
        rc=0
        with (out/(suite+'.log')).open('w') as log:
            for command in steps:
                rc=subprocess.run(command,cwd=ROOT,env=env,stdout=log,stderr=subprocess.STDOUT).returncode
                if rc:break
        result=expected_output(suite,a.tag)
        if rc==0 and (not result.is_file() or not result.stat().st_size):rc=3
        record.update(return_code=rc,status='passed' if rc==0 else 'failed',output=str(result.relative_to(ROOT)))
        manifest['status']='failed' if rc else 'running'
        (out/'manifest.json').write_text(json.dumps(manifest,indent=2)+'\n')
        if rc:print(f'{suite} failed; see {out/(suite+".log")}',file=sys.stderr);return rc
    manifest['status']='passed';(out/'manifest.json').write_text(json.dumps(manifest,indent=2)+'\n')
    print('All selected suites passed:',out);return 0

if __name__=='__main__':raise SystemExit(main())
