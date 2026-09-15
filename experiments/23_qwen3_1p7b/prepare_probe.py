#!/usr/bin/env python3
"""Prepare real Qwen traces using recorded per-tensor allocation ranges."""
from pathlib import Path
import argparse,bisect,hashlib,json,lzma,re
HERE=Path(__file__).resolve().parent
BASE=274877906944
def digest(path):
    h=hashlib.sha256()
    with path.open('rb') as stream:
        for block in iter(lambda:stream.read(1<<20),b''):h.update(block)
    return h.hexdigest()

def main() -> None:
    ap=argparse.ArgumentParser();ap.add_argument('--capture',type=Path,default=HERE/'capture_probe');ap.add_argument('--output',type=Path,default=HERE/'replay_probe');args=ap.parse_args()
    weights=json.loads((args.capture/'metadata/weights.json').read_text());weights.sort(key=lambda x:x['address']);cursor=BASE
    for w in weights:
        cursor=(cursor+4095)//4096*4096;w['target']=cursor;cursor+=w['bytes']
    for a,b in zip(weights,weights[1:]):assert a['address']+a['bytes']<=b['address'],'overlapping parameters'
    starts=[w['address'] for w in weights];args.output.mkdir(exist_ok=False)
    files=[x for x in (args.capture/'traces/kernelslist.g').read_text().splitlines() if x.startswith('kernel')];reports=[]
    for name in files:
        src=args.capture/'traces'/name;dst=args.output/name;versions=set();mapped=0
        with lzma.open(src,'rt') as f,lzma.open(dst,'wt',preset=1) as g:
            for line in f:
                if line.startswith('-binary version'):versions.add(int(line.split('=')[1]))
                t=line.split()
                if len(t)<7 or not re.fullmatch('[0-9a-fA-F]+',t[0]):g.write(line);continue
                oi=3+int(t[2]);op=t[oi];wi=oi+2+int(t[oi+1]);width=int(t[wi])
                if not width:g.write(line);continue
                mode=int(t[wi+1]);idx=wi+2;n=int(t[1],16).bit_count()
                if mode==0:addr=[int(x,16) for x in t[idx:idx+n]];end=idx+n
                elif mode==1:addr=[int(t[idx],16)+i*int(t[idx+1]) for i in range(n)];end=idx+2
                elif mode==2:
                    addr=[int(t[idx],16)]
                    for d in t[idx+1:idx+n]:addr.append(addr[-1]+int(d))
                    end=idx+n
                else:raise ValueError('address mode')
                new=[];changed=False
                for a in addr:
                    j=bisect.bisect_right(starts,a)-1;w=weights[j] if j>=0 else None
                    if w and w['address']<=a<w['address']+w['bytes']:
                        if not op.startswith(('LDG','LD.')):raise ValueError(f'parameter access by {op}')
                        assert a+width<=w['address']+w['bytes']
                        new.append(w['target']+a-w['address']);mapped+=1;changed=True
                    else:new.append(a)
                if changed:
                    shifts={b-a for a,b in zip(addr,new)}
                    if mode in (1,2) and len(shifts)==1:t[idx]=hex(new[0])
                    else:t=t[:wi+1]+['0']+[hex(a) for a in new]+t[end:]
                    line=' '.join(t)+'\n'
                g.write(line)
        reports.append(dict(file=name,binary_versions=sorted(versions),mapped_lane_accesses=mapped,input_sha256=digest(src),output_sha256=digest(dst)))
        print(name,versions,mapped,flush=True)
    (args.output/'kernelslist.g').write_text(''.join(x+'\n' for x in files))
    (args.output/'manifest.json').write_text(json.dumps(dict(weights=weights,kernels=reports,total_mapped_bytes=cursor-BASE,probe_only=args.capture.name=='capture_probe'),indent=2)+'\n')

if __name__=='__main__':main()
