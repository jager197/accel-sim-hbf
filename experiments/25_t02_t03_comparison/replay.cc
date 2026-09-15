// Fixed-arrival, read-only boundary driver for the installed HBF controller.
// GPU cores are constructed for identical configuration, but never advanced.
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include "gpgpu_context.h"
#include "gpgpu-sim/gpu-sim.h"
#include "gpgpu-sim/hbf_system.h"
#include "gpgpu-sim/mem_fetch.h"
#include "gpgpu-sim/icnt_wrapper.h"
#include "option_parser.h"
struct Request { unsigned long long at, address; unsigned bytes, sp; };
class ReplayGPU : public exec_gpgpu_sim {
 public:
  ReplayGPU(const gpgpu_sim_config &c, gpgpu_context *ctx):exec_gpgpu_sim(c,ctx){}
  void replay(const std::vector<Request>& input) {
    init();
    const auto *cfg=getMemoryConfig();
    size_t next=0, returned=0;
    unsigned long long blocked=0;
    while(next<input.size() || returned<input.size()) {
      if(gpu_sim_cycle>100000000) throw std::runtime_error("replay timeout");
      int mask=next_clock_domain();
      if(mask & 4) {
        if(m_hbf_system->busy()) m_hbf_system->cycle();
        for(unsigned sp=0;sp<cfg->m_n_mem*cfg->m_n_sub_partition_per_memory_channel; ++sp) {
          if(auto *mf=m_hbf_system->pop_return_for(sp)) {delete mf; ++returned;}
        }
        // Same partition ingress bandwidth (one request per DRAM tick).
        std::vector<bool> used(cfg->m_n_mem,false);
        while(next<input.size() && input[next].at<=gpu_sim_cycle) {
          auto r=input[next];
          if(r.sp==~0u) { addrdec_t raw; cfg->m_address_mapping.addrdec_tlx(r.address,&raw); r.sp=raw.sub_partition; }
          unsigned part=r.sp/cfg->m_n_sub_partition_per_memory_channel;
          if(used.at(part)) break;
          if(m_hbf_system->full(r.address,false)) {++blocked;break;}
          mem_access_t access(GLOBAL_ACC_R,r.address,r.bytes,false,gpgpu_ctx);
          auto *mf=new mem_fetch(access,nullptr,0,8,0,0,0,cfg,r.at);
          if(mf->get_sub_partition_id()!=r.sp) throw std::runtime_error("source mapping differs");
          m_hbf_system->push(mf); used[part]=true; ++next;
        }
      }
      if(mask & 1) ++gpu_sim_cycle;
    }
    m_hbf_system->print_stat(stdout);
    std::cout<<"REPLAY requests="<<next<<" returns="<<returned<<" end_cycle="<<gpu_sim_cycle<<" blocked_ticks="<<blocked<<"\n";
  }
};
int main(int argc,char **argv) {
 try {
  if(argc!=2) throw std::runtime_error("usage: replay input.tsv (config in cwd)");
  std::ifstream f(argv[1]); std::vector<Request> input; Request r;
  while(f>>r.at>>r.address>>r.bytes>>r.sp) {
    if(r.bytes==0 || r.bytes>128) throw std::runtime_error("input must use GPU transactions of 1..128 bytes");
    input.push_back(r);
  }
  if(input.empty() || !f.eof()) throw std::runtime_error("invalid or empty input");
  if(!std::is_sorted(input.begin(),input.end(),[](const Request&a,const Request&b){return a.at<b.at;})) throw std::runtime_error("unordered input");
  auto *ctx=new gpgpu_context;
  auto *cfg=new gpgpu_sim_config(ctx);
  auto opp=option_parser_create();
  ctx->ptx_reg_options(opp); ctx->func_sim->ptx_opcocde_latency_options(opp);
  icnt_reg_options(opp); cfg->reg_options(opp);
  const char *args[]={"replay","-config","gpgpusim.config"};
  option_parser_cmdline(opp,3,args); cfg->init();
  auto *gpu=new ReplayGPU(*cfg,ctx);gpu->replay(input);
  // Controller trace streams are flushed by exit; retain GPU for process life,
  // consistent with simulator entrypoint ownership.
  return 0;
 }catch(const std::exception&e){std::cerr<<e.what()<<"\n";return 1;}
}
