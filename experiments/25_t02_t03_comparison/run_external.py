#!/usr/bin/env python3
"""External MQSim read-page cross-check, retaining its host/protocol semantics."""
from __future__ import annotations
import csv,json,os,re,subprocess,xml.etree.ElementTree as ET
from pathlib import Path
from run_replay import ROOT,HERE,FULL,SOURCE,digest,events,measured
MQ=SOURCE/'vendor/MQSim'
def xml_set(root:ET.Element,values:dict)->None:
    for k,v in values.items():
        n=root.find('.//'+k)
        if n is None:raise ValueError(k)
        n.text=str(v)
def main()->None:
    results=[]
    for ch in (1,4):
      for tr in (1,15):
       for arrival in ('isolated','burst'):
        name=f'c{ch}_t{tr}_{arrival}';out=HERE/'external'/name;out.mkdir(parents=True,exist_ok=False)
        # 64 unique, full 4-KiB pages; integer-ns arrival trace common to both.
        times=[1000+i*(50000 if arrival=='isolated' else 10) for i in range(64)]
        (out/'requests.trace').write_text(''.join(f'{t} 0 {i*8} 8 1\n' for i,t in enumerate(times)))
        cfg=ET.parse(MQ/'ssdconfig.xml');xml_set(cfg.getroot(),dict(PCIe_Lane_Bandwidth=192,PCIe_Lane_Count=4,Enabled_Preconditioning='false',Ideal_Mapping_Table='true',IO_Queue_Depth=4096,Queue_Fetch_Size=4096,Flash_Channel_Count=ch,Chip_No_Per_Channel=1,Flash_Channel_Width=192,Channel_Transfer_Rate=1000,Die_No_Per_Chip=1,Plane_No_Per_Die=1,Block_No_Per_Plane=128,Page_No_Per_Block=256,Page_Capacity=4096,Page_Metadat_Capacity=0,Flash_Technology='SLC',Dynamic_Wearleveling_Enabled='false',Static_Wearleveling_Enabled='false',Page_Read_Latency_LSB=tr*1000,Page_Read_Latency_CSB=tr*1000,Page_Read_Latency_MSB=tr*1000,Page_Program_Latency_LSB=200000,Page_Program_Latency_CSB=200000,Page_Program_Latency_MSB=200000,Block_Erase_Latency=2000000))
        cfg.write(out/'ssdconfig.xml')
        scenarios=ET.Element('MQSim_IO_Scenarios');flow=ET.SubElement(ET.SubElement(scenarios,'IO_Scenario'),'IO_Flow_Parameter_Set_Trace_Based')
        opts=dict(Priority_Class='HIGH',Device_Level_Data_Caching_Mode='TURNED_OFF',Channel_IDs=','.join(map(str,range(ch))),Chip_IDs=0,Die_IDs=0,Plane_IDs=0,Initial_Occupancy_Percentage=0,File_Path=str(out/'requests.trace'),Percentage_To_Be_Executed=100,Relay_Count=1,Time_Unit='NANOSECOND')
        for k,v in opts.items():ET.SubElement(flow,k).text=str(v)
        ET.ElementTree(scenarios).write(out/'workload.xml')
        with (out/'mqsim.log').open('w') as f:subprocess.run([str(MQ/'MQSim'),'-i',str(out/'ssdconfig.xml'),'-w',str(out/'workload.xml')],cwd=out,stdout=f,stderr=subprocess.STDOUT,stdin=subprocess.DEVNULL,check=True,timeout=60)
        # HBF fixed-arrival input has no GPU pipeline; tick frequencies exactly
        # represent integer ns (1GHz) and media parameters use the same units.
        s=(FULL/'configs/read_p16_e32_q128_t12750_m1.config').read_text()
        changes=dict(gpgpu_clock_domains='1000:1000:1000:1000',gpgpu_hbf_num_channels=ch,gpgpu_hbf_num_subarrays=ch,gpgpu_hbf_max_active=ch,gpgpu_hbf_max_outstanding=4096,gpgpu_hbf_mshr_enabled=1,gpgpu_hbf_buffer_enabled=0,gpgpu_hbf_tR=tr*1000,gpgpu_hbf_tPROG=200000,gpgpu_hbf_tBERS=2000000,gpgpu_hbf_trace_file=f'"{out}/hbf.csv"')
        for k,v in changes.items():s=re.sub(r'^-'+k+r'\s+.*$',f'-{k} {v}',s,flags=re.M)
        (out/'gpgpusim.config').write_text(s)
        (out/'input.tsv').write_text(''.join(f'{t} {274877906944+i*4096+j*128} 128 4294967295\n' for i,t in enumerate(times) for j in range(32)))
        (out/'config_volta_islip.icnt').symlink_to(ROOT/'gpu-simulator/gpgpu-sim/configs/tested-cfgs/SM7_QV100/config_volta_islip.icnt')
        with (out/'hbf.log').open('w') as f:subprocess.run([str(SOURCE/'replay'),str(out/'input.tsv')],cwd=out,stdout=f,stderr=subprocess.STDOUT,check=True,timeout=60)
        hrows=events(out/'hbf.csv');h=measured(hrows);assert h['requests']==2048 and h['bytes']==64*4096 and h['page_services']==64
        # Report generated/serviced bytes, not just MQSim's process status.
        reports=list(out.glob('*scenario*xml'));assert len(reports)==1,reports
        tree=ET.parse(reports[0]);flowout=tree.find('./Host/Host.IO_Flow');assert flowout is not None
        attr={n.tag:n.text for n in flowout};assert int(attr['Request_Count'])==64 and float(attr['Bytes_Transferred'])==64*4096
        end_ns=64/float(attr['IOPS'])*1e9
        chips=[n for n in tree.iter() if 'Fraction_of_Time_in_Execution' in n.attrib]
        util=sum(float(n.attrib['Fraction_of_Time_in_Execution']) for n in chips)/len(chips)
        row=dict(name=name,channels=ch,tr_us=tr,arrival=arrival,hbf_span_ns=h['span'],mqsim_span_ns=end_ns-times[0],gap_pct=100*((end_ns-times[0])/h['span']-1),hbf_mean_ns=sum(max(int(r['sim_cycle']) for r in hrows if r['state']=='COMPLETED' and int(r['page'])==i)-times[i] for i in range(64))/64,mqsim_device_mean_us=int(attr['Device_Response_Time']),mqsim_host_mean_us=int(attr['End_to_End_Request_Delay']),mqsim_execution_util=util,hbf_nominal_array_duty_factor=64*tr*1000/ch/h['last'],requests=64,payload_bytes=64*4096)
        results.append(row);print(row,flush=True)
    with (HERE/'external_summary.csv').open('w',newline='') as f:w=csv.DictWriter(f,fieldnames=list(results[0]));w.writeheader();w.writerows(results)
    (HERE/'external_provenance.json').write_text(json.dumps(dict(repository='https://github.com/CMU-SAFARI/MQSim',commit=subprocess.check_output(['git','rev-parse','HEAD'],cwd=MQ,text=True).strip(),binary_sha256=digest(MQ/'MQSim'),git_diff=subprocess.check_output(['git','diff'],cwd=MQ,text=True),driver_sha256=digest(Path(__file__)),hbf_binary_sha256=digest(SOURCE/'replay')),indent=2)+'\n')
if __name__=='__main__':main()
