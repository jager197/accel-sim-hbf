from pathlib import Path
import argparse
import os,subprocess,json,re
HERE=Path(__file__).resolve().parent;ROOT=HERE.parents[1]
ap=argparse.ArgumentParser();ap.add_argument('--trace-dir',type=Path,default=HERE/'replay_smoke');ap.add_argument('--output',type=Path,default=HERE/'smoke_result');args=ap.parse_args()
out=args.output.resolve();out.mkdir(parents=True,exist_ok=False)
base=ROOT/'artifact/configs/qwen3.config'
text=base.read_text()
text=re.sub(r'(?m)^-gpgpu_hbf_trace_file\s+.*$', f'-gpgpu_hbf_trace_file "{out}/run/hbf.csv"',text)
config=out/'gpgpusim.config';config.write_text(text)
listing=args.trace_dir.resolve()/'kernelslist.g'
gs=ROOT/'gpu-simulator/gpgpu-sim';lib=next(gs.glob('lib/gcc-*/cuda-*/release/libcudart.so'))
env=dict(os.environ,CUDA_INSTALL_PATH=os.environ.get('CUDA_INSTALL_PATH','/usr/local/cuda'),GPGPUSIM_ROOT=str(gs),REPO_ROOT=str(ROOT),ICXT_CFG=str(gs/'configs/tested-cfgs/SM7_QV100/config_volta_islip.icnt'),LD_LIBRARY_PATH=str(lib.parent),RUN_TIMEOUT='172800')
binary=ROOT/'gpu-simulator/bin/release/accel-sim.out'
subprocess.run(['bash',str(ROOT/'experiments/common/run_sim.sh'),str(out/'run'),str(config),str(binary),'-config',str(config),'-trace',str(listing)],env=env,check=True)
