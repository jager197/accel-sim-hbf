#!/usr/bin/env python3
"""Continue the actual full capture into grouped traces, remapping and replay."""
from pathlib import Path
import json,subprocess,time,os,sys
HERE=Path(__file__).resolve().parent
status=HERE/'full_status.json'
def state(stage):
    status.write_text(json.dumps(dict(stage=stage,time=time.strftime('%Y-%m-%dT%H:%M:%SZ',time.gmtime()),pid=os.getpid()),indent=2)+'\n')
def run(args,log):
    with (HERE/log).open('w') as f:subprocess.run(args,cwd=HERE,stdout=f,stderr=subprocess.STDOUT,check=True)
try:
    state('waiting_for_full_capture')
    start=time.monotonic()
    while not (HERE/'capture_full/metadata/result.json').is_file():
        if time.monotonic()-start>21600:raise TimeoutError('capture exceeded six-hour continuation window')
        time.sleep(30)
    import torch
    x=torch.load(HERE/'native/logits.pt');y=torch.load(HERE/'capture_full/metadata/logits.pt')
    assert torch.equal(x,y),'full capture changes logits'
    report=json.loads((HERE/'capture_full/metadata/result.json').read_text());assert report['probe_layer']==-1 and report['capture_step']==2
    state('postprocessing_full_capture')
    lists=list((HERE/'capture_full/traces').glob('kernelslist_ctx_*'));assert len(lists)==1
    run([str(HERE.parents[1]/'util/tracer_nvbit/tracer_tool/traces-processing/post-traces-processing'),str(lists[0])],'postprocess_full.log')
    state('mapping_full_capture')
    run([sys.executable,'prepare_probe.py','--capture','capture_full','--output','replay_full'],'prepare_full.log')
    state('replaying_full_decode')
    run([sys.executable,'run_smoke.py','--trace-dir','replay_full','--output','full_result'],'replay_full.log')
    state('replay_finished_validation_pending')
except BaseException as exc:
    status.write_text(json.dumps(dict(stage='failed',error=str(exc),pid=os.getpid()),indent=2)+'\n')
    raise
