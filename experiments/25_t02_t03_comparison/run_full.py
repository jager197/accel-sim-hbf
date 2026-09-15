#!/usr/bin/env python3
"""Launch the predeclared 24-point Full matrix with the existing runner."""
import subprocess,os,sys
from pathlib import Path
ROOT=Path(__file__).resolve().parents[2]
names=[f'read_p16_e{e}_q{q}_t{t}_m{m}' for e in (1,32) for q in (32,128,4096) for t in (850,12750) for m in (0,1)]
if __name__=='__main__':
    subprocess.run([sys.executable,str(ROOT/'experiments/17_system_boundaries/run.py'),'--tag',os.environ.get('RUN_TAG','comparison-001'),'--jobs',os.environ.get('MAX_PAR','1'),'--cases',*names],cwd=ROOT,check=True)
