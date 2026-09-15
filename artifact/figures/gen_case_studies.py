#!/usr/bin/env python3
"""Publication figures for the platform and two case studies; frozen CSV inputs."""
from __future__ import annotations
import argparse
import csv
import hashlib
import json
from pathlib import Path
import statistics
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
from matplotlib.patches import Rectangle
import numpy as np
CHANNELS=['#0072B2','#E69F00','#009E73','#CC79A7']

HERE=Path(__file__).resolve().parent
DATA=HERE.parent/'reference'
COLORS=['#0072B2','#E69F00','#009E73','#CC79A7']
HATCHES=['///','...','xxx','---']
NAMES=['Interleave','Grouped (2)','Capacity-contiguous','Balanced regions']
POLICIES=['interleave','grouped','contiguous','balanced']
plt.rcParams.update({'font.family':'serif','font.serif':['DejaVu Serif'],'font.size':8,
    'axes.labelsize':8,'axes.titlesize':8.5,'legend.fontsize':7,'legend.frameon':False,
    'axes.spines.top':False,'axes.spines.right':False,'axes.axisbelow':True,'pdf.fonttype':42})


def load(name: str) -> list[dict]:
    return list(csv.DictReader((DATA/name).open()))


def save(fig: plt.Figure,stem: str) -> None:
    for ext in ('pdf','png'):
        fig.savefig(HERE/(stem+'.'+ext),bbox_inches='tight',pad_inches=.045,dpi=240,
                    metadata={'CreationDate':None,'ModDate':None} if ext=='pdf' else None)
    plt.close(fig)


def placement_map() -> None:
    fig,ax=plt.subplots(figsize=(3.45,2.3));ax.set_xlim(0,1);ax.set_ylim(0,1);ax.axis('off')
    ax.text(.32,.98,'Active logical pages 0–15',fontsize=8,va='top')
    mappings=[[i%4 for i in range(16)],[(i//2)%4 for i in range(16)],[0]*16,[i//4 for i in range(16)]]
    labels=['Interleave','Grouped (2)','Capacity-\ncontiguous','Balanced\nregions']
    for row,(mapping,label) in enumerate(zip(mappings,labels)):
        y=.76-row*.18
        ax.text(.0,y+.04,label,fontsize=7,va='center')
        for i,ch in enumerate(mapping):
            x=.31+i*.042
            ax.add_patch(Rectangle((x,y),.039,.08,facecolor=CHANNELS[ch],edgecolor='white',lw=.4))
            ax.text(x+.0195,y+.04,str(i),fontsize=5.4,color='white' if ch in (0,3) else '#182026',ha='center',va='center')
    for ch,color in enumerate(CHANNELS):
        x=.29+ch*.17;ax.add_patch(Rectangle((x,.07),.033,.038,facecolor=color))
        ax.text(x+.041,.089,f'Ch {ch}',fontsize=6.5,va='center')
    fig.subplots_adjust(left=.025,right=.99,top=.97,bottom=.03);save(fig,'case_placement_map')


def placement_results() -> None:
    rows=load('placement_cases.csv')
    expected={(tr,p,e) for tr in (20,12750) for p in POLICIES for e in (1,8,32)}
    data={(int(r['tr']),r['placement'],int(r['entries'])):r for r in rows}
    if len(rows)!=24 or set(data)!=expected:raise ValueError('incomplete placement matrix')
    if any(r['run_rc']!='0' or r['trace_valid']!='1' for r in rows):raise ValueError('failed placement cell')
    if len({r['simulator_sha256'] for r in rows})!=1:raise ValueError('mixed binaries')
    fig,axes=plt.subplots(2,2,figsize=(6.8,3.75));es=[1,8,32];x=np.arange(3);width=.19
    for row,tr in enumerate((20,12750)):
        left,right=axes[row]
        for j,(p,label,color,hatch) in enumerate(zip(POLICIES,NAMES,COLORS,HATCHES)):
            records=[data[(tr,p,e)] for e in es]
            left.bar(x+(j-1.5)*width,[float(r['page_service_amplification']) for r in records],width=width,color=color,hatch=hatch,edgecolor='white',linewidth=.5,label=label)
            right.plot(x,[float(r['cycles'])/float(data[(tr,'interleave',e)]['cycles']) for r,e in zip(records,es)],marker=['o','s','^','D'][j],ms=4,color=color,lw=1.3,label=label)
        left.plot(x,[32/e for e in es],color='#333333',ls=':',marker='_',lw=1.2,label='Array amplification (all)')
        left.set_ylabel('Page-service amplification');left.set_ylim(0,36)
        right.axhline(1,color='.5',ls=':',lw=.8);right.set_ylabel('Kernel time / interleave')
        right.set_ylim((.985,1.02) if row==0 else (.999,1.004))
        right.ticklabel_format(axis='y',style='plain',useOffset=False)
        for ax in (left,right):ax.set_xticks(x,es);ax.set_xlabel('128 B entries per page');ax.grid(axis='y',alpha=.18)
        left.set_title(('(a) Shortened timing: page service','(c) Default timing: page service')[row],loc='left')
        right.set_title(('(b) Shortened timing: execution','(d) Default timing: execution')[row],loc='left')
    handles,labels=axes[0,0].get_legend_handles_labels()
    fig.legend(handles,labels,loc='upper center',ncol=3,bbox_to_anchor=(.5,1.03),fontsize=7)
    fig.tight_layout(rect=(0,0,1,.91),pad=.6,h_pad=1,w_pad=1.3);save(fig,'case_placement_results')


def platform_cost() -> None:
    rows=load('cost_overhead_audited.csv');fig,axes=plt.subplots(1,2,figsize=(3.45,1.9))
    for ax,key,unit,title in zip(axes,['wall_seconds','max_rss_kib'],[1,1024],['Host time (s)','Peak RSS (MiB)']):
        for i,mode in enumerate(['disabled','enabled']):
            vals=[float(r[key])/unit for r in rows if r['mode']==mode]
            ax.scatter(i+np.linspace(-.06,.06,len(vals)),vals,s=15,color=COLORS[i],zorder=3)
            ax.hlines(statistics.median(vals),i-.18,i+.18,color='black',lw=1.1,zorder=4)
        ax.set_xticks([0,1],['Off','On']);ax.set_xlim(-.45,1.45);ax.set_ylabel(title);ax.grid(axis='y',alpha=.2);ax.set_xlabel('Idle HBF path')
    fig.tight_layout(pad=.5,w_pad=.8);save(fig,'platform_cost')


def write_tradeoff() -> None:
    data={r['name']:r for r in load('phase_boundary_summary.csv')}
    configs=[(0,1,'Shared: FCFS / RP / drain'),(1,1,'1R/3W'),(2,1,'2R/2W'),(3,1,'3R/1W')]
    fig,ax=plt.subplots(figsize=(3.45,2.35));baseline=float(data['phase_w4_d113200_r0_s1']['cycles']);alone=float(data['phase_alone_d113200']['read_total_p95'])
    for sched in (0,2):
        d=data[f'phase_w4_d113200_r0_s{sched}']
        if float(d['cycles']) != baseline or float(d['read_total_p95']) != float(data['phase_w4_d113200_r0_s1']['read_total_p95']):
            raise ValueError('shared policies no longer coincide')
    offsets=[(-10,10),(-32,8),(5,-2),(5,-12)]
    for idx,((r,s,label),offset) in enumerate(zip(configs,offsets)):
        d=data[f'phase_w4_d113200_r{r}_s{s}'];x=float(d['cycles'])/baseline;y=float(d['read_total_p95'])/alone
        ax.scatter(x,y,s=30,c=[COLORS[idx]],marker='o' if r==0 else 's',zorder=3)
        ax.annotate(label,(x,y),xytext=offset,textcoords='offset points',fontsize=6.6)
    ax.axhline(1.5,color='.5',ls=':',lw=1);ax.set_xlabel('Kernel time / shared read priority');ax.set_ylabel('Read p95 / matched read alone');ax.grid(alpha=.18)
    ax.margins(x=.18,y=.2);fig.tight_layout(pad=.5);save(fig,'case_write_tradeoff')


def main() -> None:
    global HERE, DATA
    source_dir = Path(__file__).resolve().parent
    parser = argparse.ArgumentParser()
    parser.add_argument('--output-dir', type=Path, default=HERE)
    parser.add_argument('--data-dir', type=Path, default=DATA)
    args = parser.parse_args()
    DATA = args.data_dir.resolve()
    HERE = args.output_dir.resolve()
    HERE.mkdir(parents=True, exist_ok=True)
    placement_map();placement_results();platform_cost();write_tradeoff()
    inputs=['placement_cases.csv','cost_overhead_audited.csv','phase_boundary_summary.csv']
    outputs=['case_placement_map','case_placement_results','platform_cost','case_write_tradeoff']
    digest=lambda p:hashlib.sha256(p.read_bytes()).hexdigest()
    manifest=dict(inputs={p:digest(DATA/p) for p in inputs},generators={p:digest(source_dir/p) for p in ['gen_case_studies.py']},
        outputs={s+'.'+e:digest(HERE/(s+'.'+e)) for s in outputs for e in ('pdf','png')})
    (HERE/'case_studies_manifest.json').write_text(json.dumps(manifest,indent=2)+'\n')

if __name__=='__main__':main()
