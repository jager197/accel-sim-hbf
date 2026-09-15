#!/usr/bin/env python3
"""Run the authorized 1.7B replacement pipeline, recording each stage."""
from pathlib import Path
import hashlib,json,os,subprocess,sys,time
HERE=Path(__file__).resolve().parent
STATUS=HERE/'full_status.json'
def state(stage):
    STATUS.write_text(json.dumps(dict(stage=stage,model='Qwen/Qwen3-1.7B',pid=os.getpid(),time=time.strftime('%Y-%m-%dT%H:%M:%SZ',time.gmtime())),indent=2)+'\n')
def run(args,log,env=None,cwd=HERE):
    with (HERE/log).open('w') as f:subprocess.run(args,cwd=cwd,env=env,stdout=f,stderr=subprocess.STDOUT,check=True)
try:
    state('downloading_model')
    run([sys.executable,'download_model.py'],'download.log',dict(os.environ,HF_HUB_DISABLE_XET='1'))
    state('verifying_model')
    metadata=json.loads((HERE/'model_metadata.json').read_text());records=[]
    for item in metadata['siblings']:
        if not item['rfilename'].endswith('.safetensors'):continue
        path=HERE/'model'/item['rfilename'];h=hashlib.sha256()
        with path.open('rb') as f:
            for b in iter(lambda:f.read(1<<22),b''):h.update(b)
        assert h.hexdigest()==item['lfs']['sha256'],item['rfilename']
        records.append(dict(file=item['rfilename'],sha256=h.hexdigest(),bytes=path.stat().st_size))
    (HERE/'model_validation.json').write_text(json.dumps(dict(valid=True,revision=metadata['sha'],files=records),indent=2)+'\n')
    state('native_inference')
    run([sys.executable,'run_model.py','--output','native','--tokens','8'],'native.log')
    state('capturing_full_decode')
    cap=HERE/'capture_full';cap.mkdir(exist_ok=False)
    env=dict(os.environ,ACTIVE_FROM_START='0',LD_PRELOAD=str(HERE.parents[1]/'util/tracer_nvbit/tracer_tool/tracer_tool.so'))
    run([sys.executable,str(HERE/'run_model.py'),'--output','metadata','--tokens','4','--capture-step','2'],'capture_full/capture.log',env,cap)
    os.execv(sys.executable,[sys.executable,str(HERE/'continue_full.py')])
except BaseException as exc:
    STATUS.write_text(json.dumps(dict(stage='failed',error=str(exc),pid=os.getpid()),indent=2)+'\n')
    raise
