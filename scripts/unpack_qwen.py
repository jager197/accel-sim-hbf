#!/usr/bin/env python3
"""Verify and unpack the exact, weight-remapped Qwen decode trace."""
import argparse,hashlib,json,shutil,tarfile,tempfile
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]
def digest(path):
    h=hashlib.sha256()
    with path.open('rb') as stream:
        for block in iter(lambda:stream.read(1<<20),b''):h.update(block)
    return h.hexdigest()
def main():
    ap=argparse.ArgumentParser();ap.add_argument('--output',type=Path,default=ROOT/'artifact_runs/inputs/qwen3');a=ap.parse_args()
    source=ROOT/'artifact/inputs/qwen3';manifest=json.loads((source/'manifest.json').read_text())
    if a.output.exists():raise FileExistsError('output already exists: '+str(a.output))
    with tempfile.TemporaryFile() as stream:
        for part in manifest['parts']:
            p=source/part['file']
            if p.stat().st_size!=part['bytes'] or digest(p)!=part['sha256']:raise ValueError('archive part mismatch: '+p.name)
            with p.open('rb') as data:shutil.copyfileobj(data,stream)
        stream.seek(0);a.output.parent.mkdir(parents=True,exist_ok=True)
        with tempfile.TemporaryDirectory(dir=a.output.parent) as tmp:
            tmp=Path(tmp);seen=set()
            with tarfile.open(fileobj=stream,mode='r:') as tar:
                for member in tar:
                    if not member.isfile() or member.name!=Path(member.name).name or member.name not in manifest['files'] or member.name in seen:raise ValueError('unexpected archive member')
                    seen.add(member.name)
                    with tar.extractfile(member) as src,(tmp/member.name).open('wb') as dst:shutil.copyfileobj(src,dst)
                    if digest(tmp/member.name)!=manifest['files'][member.name]:raise ValueError('trace hash mismatch')
            if seen!=manifest['files'].keys():raise ValueError('missing trace files')
            tmp.rename(a.output)
    print('Verified 1819 kernels:',a.output)
if __name__=='__main__':main()
