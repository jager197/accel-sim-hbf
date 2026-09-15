#!/usr/bin/env python3
"""Array service scaling and explicit request-supply controls."""
from __future__ import annotations
import argparse
import csv
import hashlib
import json
from pathlib import Path
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
HERE=Path(__file__).resolve().parent
DATA=HERE.parent/'reference/media_saturation.csv'
plt.rcParams.update({'font.family':'serif','font.size':8,'axes.labelsize':8,
 'axes.titlesize':8.5,'legend.fontsize':7,'legend.frameon':False,
 'axes.spines.top':False,'axes.spines.right':False,'pdf.fonttype':42})


def generate(data:Path,out:Path)->None:
    rows=list(csv.DictReader(data.open()));d={r['name']:r for r in rows}
    if len(rows)!=9 or len(d)!=9:raise ValueError('incomplete saturation study')
    if any(r['run_rc']!='0' or r['trace_valid']!='1' for r in rows):raise ValueError('invalid run')
    if len({r['simulator_sha256'] for r in rows})!=1:raise ValueError('mixed simulator builds')
    main=[d[f'grow_c{c}_p1024_n1024'] for c in [1,2,4,8,16]]
    if any(float(r['media_utilization'])<.99 or int(r['peak_active'])!=int(r['active']) for r in main):raise ValueError('main sweep not saturated')
    out.mkdir(parents=True,exist_ok=True)
    fig,axes=plt.subplots(1,3,figsize=(6.9,2.25))
    x=[1,2,4,8,16]
    to_mpps=lambda r:float(r['array_bandwidth_gbs'])/4.096
    axes[0].plot(x,[to_mpps(r) for r in main],color='#0072B2',marker='o',label='32 slots / channel',lw=1.5,ms=4)
    axes[0].plot(x,[float(r['array_ceiling_gbs'])/4.096 for r in main],color='.4',ls=':',label='Configured ceiling',lw=1.3)
    fixed=d['fixed_c16_p1024_n1024'];axes[0].scatter([16],[to_mpps(fixed)],color='#D55E00',marker='x',s=32,label='32 total slots')
    axes[0].set_xticks(x);axes[0].set_xlabel('Channels');axes[0].set_ylabel('Array reads (million pages/s)');axes[0].set_ylim(0,40); axes[0].secondary_yaxis('right', functions=(lambda v:v*4.096, lambda v:v/4.096)).set_ylabel('Array bandwidth (GB/s)');axes[0].legend(loc='upper left',fontsize=5.8)
    axes[0].set_title('(a) Media resource scaling',loc='left')
    util=[100*float(r['media_utilization']) for r in main]
    axes[1].bar(range(5),util,color='#009E73',width=.62)
    axes[1].set_xticks(range(5),x);axes[1].set_xlabel('Channels');axes[1].set_ylabel('Mean active-slot occupancy (%)');axes[1].set_ylim(0,114)
    for i,v in enumerate(util):axes[1].text(i,v+2,f'{v:.1f}',ha='center',fontsize=6)
    axes[1].set_title('(b) Supplied media slots',loc='left')
    controls=[d['supply_c16_p1024_n256'],d['supply_c16_p1024_n512'],main[-1]]
    axes[2].plot([256,512,1024],[to_mpps(r) for r in controls],color='#E69F00',marker='s',ms=4,lw=1.5,label='1,024 pages')
    axes[2].axhline(512/15,color='.4',ls=':',lw=1.3)
    axes[2].set_xticks([256,512,1024]);axes[2].set_xlabel('Independent lane producers');axes[2].set_ylabel('Array reads (million pages/s)');axes[2].set_ylim(0,40); axes[2].secondary_yaxis('right', functions=(lambda v:v*4.096, lambda v:v/4.096)).set_ylabel('Array bandwidth (GB/s)')
    axes[2].set_title('(c) Supply check: 16 channels',loc='left');axes[2].legend(fontsize=6,loc='lower right')
    for ax in axes:ax.grid(axis='y',alpha=.18);ax.set_axisbelow(True)
    fig.tight_layout(pad=.55,w_pad=1.2)
    for ext in ['pdf','png']:
        fig.savefig(out/('media_saturation.'+ext),bbox_inches='tight',dpi=240,metadata={'CreationDate':None,'ModDate':None} if ext=='pdf' else None)
    plt.close(fig)
    manifest=dict(input_sha256=hashlib.sha256(data.read_bytes()).hexdigest(),generator_sha256=hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),rows=9,
        outputs={e:hashlib.sha256((out/('media_saturation.'+e)).read_bytes()).hexdigest() for e in ['pdf','png']},
        metric='unique array reads / first ingress to last controller return; not GPU payload bandwidth',occupancy='sum of configured READ busy intervals divided by slots and first-to-last media interval')
    (out/'media_saturation_manifest.json').write_text(json.dumps(manifest,indent=2)+'\n')

if __name__=='__main__':
    ap=argparse.ArgumentParser();ap.add_argument('--data',type=Path,default=DATA);ap.add_argument('--output-dir',type=Path,default=HERE);a=ap.parse_args();generate(a.data,a.output_dir)
