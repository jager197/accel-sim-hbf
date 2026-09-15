#!/usr/bin/env python3
"""Figures generated only from validated CSV outputs."""
from pathlib import Path
import csv,argparse
import matplotlib
matplotlib.use('Agg')
import matplotlib.pyplot as plt
ap=argparse.ArgumentParser();ap.add_argument('--data-dir',required=True,type=Path);ap.add_argument('--output-dir',required=True,type=Path);args=ap.parse_args()
HERE=args.data_dir.resolve();OUT=args.output_dir.resolve();OUT.mkdir(parents=True,exist_ok=True)
plt.rcParams.update({'font.family':'serif','font.size':9,'axes.spines.top':False,'axes.spines.right':False,'pdf.fonttype':42,'ps.fonttype':42})
with (HERE/'gain_summary.csv').open() as f:rows=list(csv.DictReader(f))
# Draw at final single-column size so text stays 8--9 pt in the manuscript.
fig,axes=plt.subplots(1,2,figsize=(3.4,2.55),sharey=True)
fig.subplots_adjust(left=.19,right=.97,bottom=.24,top=.71,wspace=.15)
for ax,tr,title in zip(axes,(1,15),(r'(a) 1 $\mu$s read',r'(b) 15 $\mu$s read')):
    r=sorted([r for r in rows if int(r['entries'])==32 and float(r['tr_us'])==tr],key=lambda r:int(r['depth']))
    for key,label,color,mark in [('full_gain_pct','Closed-loop GPU','#0072B2','o'),('replay_gain_pct','Frozen ingress (MSHR off)','#D55E00','s')]:
        vals=[float(v[key]) for v in r]
        ax.plot(range(3),vals,color=color,marker=mark,label=label,lw=1.4,markersize=4)
        for i,v in enumerate(vals):
            if i<2:ax.annotate(f'{v:.1f}',(i,v),xytext=(0,5),textcoords='offset points',ha='center',fontsize=8,color=color)
    ax.set_xticks(range(3),['32','128','4,096'])
    ax.tick_params(axis='both',labelsize=8,pad=3)
    ax.set_title(title,fontsize=9,pad=7)
    ax.set_ylim(-5,103)
    ax.set_xlim(-.25,2.32)
    ax.set_yticks([0,20,40,60,80,100])
    ax.grid(axis='y',alpha=.18)
axes[0].set_ylabel('Window reduction (%)',fontsize=9,labelpad=4)
fig.supxlabel('Admission cap (page operations)',fontsize=9,y=.055)
handles,labels=axes[0].get_legend_handles_labels()
fig.legend(handles,labels,loc='upper center',bbox_to_anchor=(.55,1.015),fontsize=8,frameon=False,ncol=1,labelspacing=.3,handlelength=2)
for ext in('pdf','png'):fig.savefig(OUT/f'mshr_feedback.{ext}',dpi=300)
plt.close(fig)
with (HERE/'external_summary.csv').open() as f:rows=list(csv.DictReader(f))
fig,axes=plt.subplots(1,2,figsize=(6.8,2.5),layout='constrained')
for ax,tr in zip(axes,(1,15)):
    r=sorted([r for r in rows if r['arrival']=='burst' and int(r['tr_us'])==tr],key=lambda r:int(r['channels']))
    for key,label,color,offset in [('hbf_span_ns','HBF-Sim','#0072B2',-.18),('mqsim_span_ns','MQSim','#D55E00',.18)]:
        vals=[float(v[key])/1000 for v in r];bars=ax.bar([i+offset for i in range(2)],vals,.34,color=color,label=label)
        ax.bar_label(bars,fmt='%.2f',fontsize=8,padding=2)
    ax.set_xticks(range(2),['1 channel','4 channels']);ax.set_title(f'{tr} us read',fontsize=10);ax.set_ylabel('Burst completion window (us)');ax.margins(y=.23);ax.legend(frameon=False,fontsize=8)
for ext in('pdf','png'):fig.savefig(OUT/f'mqsim_crosscheck.{ext}',dpi=300)
