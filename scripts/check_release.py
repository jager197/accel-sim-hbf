#!/usr/bin/env python3
"""Check the distributable source set, immutable inputs and portable runners."""
from __future__ import annotations
import ast
import hashlib
import importlib.util
import json
from pathlib import Path
import subprocess
import tempfile

ROOT=Path(__file__).resolve().parents[1]

def digest(path: Path) -> str:
    h=hashlib.sha256()
    with path.open('rb') as stream:
        for block in iter(lambda:stream.read(1<<20),b''):h.update(block)
    return h.hexdigest()

def load(name: str, path: str):
    spec=importlib.util.spec_from_file_location(name,ROOT/path)
    module=importlib.util.module_from_spec(spec);spec.loader.exec_module(module)
    return module

def main() -> None:
    files=subprocess.check_output(['git','ls-files','--cached','--others','--exclude-standard','-z'],cwd=ROOT).decode().split('\0')
    files=sorted(set(x for x in files if x))
    if not files:raise ValueError('empty release file set')
    for name in files:
        p=ROOT/name
        if name.startswith(('paper/','docs/')):raise ValueError('private directory is tracked: '+name)
        if p.is_file() and p.stat().st_size>=50*1024*1024:raise ValueError('oversized source artifact: '+name)
        if name.startswith(('scripts/','experiments/','artifact/figures/','util/traces/','util/hbf/')):
            if p.suffix=='.py':ast.parse(p.read_text(),filename=name)
            if p.suffix=='.sh':subprocess.run(['bash','-n',str(p)],check=True)
            if p.suffix in ('.py','.sh','.config') and ('/'+'inspire'+'/') in p.read_text():raise ValueError('host-specific path in '+name)
    for folder in ('reference','configs'):
        manifest=ROOT/'artifact'/folder/('manifest.json' if folder=='reference' else 'provenance.json')
        for name,record in json.loads(manifest.read_text()).items():
            if digest(manifest.parent/name)!=record['sha256']:raise ValueError('input hash mismatch: '+name)
    source=ROOT/'artifact/inputs/qwen3';manifest=json.loads((source/'manifest.json').read_text())
    for part in manifest['parts']:
        p=source/part['file']
        if p.stat().st_size!=part['bytes'] or digest(p)!=part['sha256']:raise ValueError('Qwen archive mismatch')
    if len([n for n in manifest['files'] if n.endswith('.traceg.xz')])!=1819:raise ValueError('Qwen kernel set incomplete')
    # Materialize every selected portable configuration without starting a simulator.
    with tempfile.TemporaryDirectory() as tmp:
        modules=[load('release_boundary','experiments/17_system_boundaries/run.py'),
                 load('release_media','experiments/19_media_saturation/run.py'),
                 load('release_placement','experiments/18_placement_cases/run.py'),
                 load('release_hotspots','experiments/20_placement_hotspots/run.py')]
        counts=[]
        for i,module in enumerate(modules):
            out=Path(tmp)/str(i);(out/'configs').mkdir(parents=True)
            cases=module.matrix()
            for case in cases:
                config=module.configure(case,out)
                if not config.is_file() or not config.stat().st_size:raise ValueError('empty config')
            counts.append(len(cases))
    print(f'release checks: {len(files)} files; reference/config/archive hashes; portable config counts {counts}')

if __name__=='__main__':main()
