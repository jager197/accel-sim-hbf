#!/usr/bin/env python3
"""Native Qwen3-1.7B inference and opt-in capture of a full real decode step."""
from pathlib import Path
import argparse,json,os,time
import torch
from transformers import AutoModelForCausalLM,AutoTokenizer
HERE=Path(__file__).resolve().parent

def main() -> None:
    ap=argparse.ArgumentParser();ap.add_argument('--output',type=Path,required=True);ap.add_argument('--capture-step',type=int,default=-1);ap.add_argument('--tokens',type=int,default=8);ap.add_argument('--probe-layer',type=int,default=-1);args=ap.parse_args()
    out=args.output.resolve();out.mkdir(parents=True,exist_ok=False)
    torch.manual_seed(0)
    model=AutoModelForCausalLM.from_pretrained(HERE/'model',torch_dtype=torch.bfloat16,attn_implementation='eager',local_files_only=True).cuda().eval()
    tokenizer=AutoTokenizer.from_pretrained(HERE/'model',local_files_only=True)
    prompt='Explain in three sentences why larger GPU memory helps large language model inference.'
    text=tokenizer.apply_chat_template([{'role':'user','content':prompt}],tokenize=False,add_generation_prompt=True,enable_thinking=False)
    inputs=tokenizer(text,return_tensors='pt').to('cuda')
    allocations=[]
    for name,p in model.named_parameters():
        allocations.append(dict(name=name,address=p.data_ptr(),bytes=p.numel()*p.element_size(),shape=list(p.shape),dtype=str(p.dtype)))
    (out/'weights.json').write_text(json.dumps(allocations,indent=2)+'\n')
    generated=[];latencies=[];cache=None;token=inputs['input_ids'];start=time.monotonic()
    active_step={'value':-1}
    if args.probe_layer>=0:
        if args.capture_step<1:raise ValueError('layer probe requires a decode capture step')
        def start_layer(module,inputs):
            if active_step['value']==args.capture_step:torch.cuda.synchronize();torch.cuda.profiler.start()
        def stop_layer(module,inputs,outputs):
            if active_step['value']==args.capture_step:torch.cuda.synchronize();torch.cuda.profiler.stop()
        model.model.layers[args.probe_layer].register_forward_pre_hook(start_layer)
        model.model.layers[args.probe_layer].register_forward_hook(stop_layer)
    with torch.inference_mode():
        for step in range(args.tokens):
            active_step['value']=step
            torch.cuda.synchronize()
            if step==args.capture_step and args.probe_layer<0:torch.cuda.profiler.start()
            begin=time.monotonic()
            outputs=model(input_ids=token,past_key_values=cache,use_cache=True)
            torch.cuda.synchronize()
            latencies.append(time.monotonic()-begin)
            if step==args.capture_step and args.probe_layer<0:torch.cuda.profiler.stop()
            cache=outputs.past_key_values
            logits=outputs.logits[:,-1,:]
            if step==args.capture_step or (args.capture_step<0 and step==2):
                torch.save(logits.float().cpu(),out/'logits.pt')
                (out/'step.json').write_text(json.dumps(dict(step=step,input_ids=token.cpu().tolist(),context_tokens=inputs['input_ids'].shape[1]+step,kind='prefill' if step==0 else 'decode'),indent=2)+'\n')
            token=logits.argmax(dim=-1,keepdim=True);generated.append(token.item())
    report=dict(model='Qwen/Qwen3-1.7B',revision=json.loads((HERE/'model_source.json').read_text())['revision'],precision='bfloat16',attention='eager',batch=1,prompt=prompt,prompt_tokens=inputs['input_ids'].shape[1],generated_ids=generated,text=tokenizer.decode(generated,skip_special_tokens=True),steps=args.tokens,native_step_seconds=latencies,wall_seconds=time.monotonic()-start,torch=torch.__version__,gpu=torch.cuda.get_device_name(),peak_gpu_bytes=torch.cuda.max_memory_allocated(),capture_step=args.capture_step,probe_layer=args.probe_layer)
    (out/'result.json').write_text(json.dumps(report,indent=2)+'\n');print(json.dumps(report,indent=2))

if __name__=='__main__':main()
