// =============================================================================
// HBF (High Bandwidth Flash) Simulation Module — Phase 1 Implementation
// =============================================================================
//
// Phase 1 建模思路：
//
// HBF 是一种新型 GPU 近存储（near-memory）技术：3D NAND 通过 TSV 堆叠在
// logic die 上，直接放在 GPU interposer 上跟 HBM 做邻居。Phase 1 只建模 HBF
// 的一阶效应——比 DRAM 高得多的访问延迟（~10us vs ~100ns），但带宽可比。
//
// 实现方式：
//   一个固定延迟的 FIFO 队列。请求进来时记上 ready_cycle = now + hbf_latency，
//   每周期 cycle() 把到期的请求搬到返回队列。memory_partition_unit 通过
//   is_hbf_addr() 判断地址区间，把 HBF 地址段的请求路由到这里。
//
// 接口完全照搬 dram_t（dram.h:112），所以 memory_partition_unit 可以像操作
// DRAM 一样操作 HBF——push / cycle / return_queue_top / return_queue_pop。
//
// 类关系：
//   memory_partition_unit (l2cache.h:74)
//     ├── m_dram  (dram_t*)      — DRAM 控制器
//     └── m_hbf   (hbf_ctrl_t*)  — HBF 控制器（本文件）
// =============================================================================

#include "hbf.h"
#include <stdio.h>
#include "../abstract_hardware_model.h"
#include "gpu-sim.h"
#include "l2cache.h"
#include "mem_fetch.h"
#include "mem_latency_stat.h"

// =============================================================================
// 构造函数 — 初始化固定延迟 HBF 控制器
//
// 只分配一个返回队列（fifo_pipeline），不分配 bank 数组、不分配 scheduler。
// Phase 2 才会加入 NAND 子阵列状态机、MSHR、FTL 等复杂结构。
// =============================================================================
hbf_ctrl_t::hbf_ctrl_t(unsigned int partition_id, const memory_config *config,
                       memory_stats_t *stats, memory_partition_unit *mp,
                       gpgpu_sim *gpu)
    : m_id(partition_id),
      m_config(config),
      m_stats(stats),
      m_memory_partition_unit(mp),
      m_gpu(gpu),
      m_current_outstanding(0),
      m_total_bytes_read(0),
      m_total_bytes_written(0) {
  n_reads = 0;
  n_writes = 0;
  total_read_latency = 0;
  total_write_latency = 0;
  max_queue_length = 0;
  n_cycles_queue_nonempty = 0;

  // 返回队列：请求完成 hbf_latency 等待后放在这里，等 hbf_cycle() 取走。
  // 延迟参数传 0 —— 返回路径是瞬时的，延迟全部在 latency_queue 里模拟。
  m_returnq = new fifo_pipeline<mem_fetch>("hbf_returnq", 0,
                                            config->gpgpu_dram_return_queue_size);
}

// =============================================================================
// full() — 检查 HBF 控制器是否还能接收新请求
//
// m_config->hbf_max_outstanding 控制最大排队深度。
// 0 表示无限制（此时回 false，来者不拒）。
// 调用方：hbf_cycle() 在推请求前先检查，满则停止本轮下发。
// =============================================================================
bool hbf_ctrl_t::full(bool is_write) const {
  unsigned int max_q = m_config->hbf_max_outstanding;
  if (max_q == 0) return false;
  return m_current_outstanding >= max_q;
}

// =============================================================================
// push() — 接收一个 L2 miss 请求，放入 HBF 延迟队列
//
// 这是 HBF 模拟的核心入口。调用方：memory_partition_unit::hbf_cycle()。
//
// 流程：
//   1. 用当前模拟周期 + hbf_latency 算出 ready_cycle
//   2. 把请求和 ready_cycle 打包成 hbf_delay_t 推进 m_latency_queue
//   3. 更新统计（读/写计数、字节数、最大队列深度）
//
// Phase 2 这里会变成：先查 MSHR → 如果同 page 已发出则合并 → 否则分配子阵列
// → 发出 NAND 读命令。Phase 1 只做延迟，不区分 page 也不建模并行度。
// =============================================================================
void hbf_ctrl_t::push(mem_fetch *data) {
  unsigned long long current_cycle =
      m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle;

  // 打包延迟条目：记录请求指针 + 到期时间
  hbf_delay_t d;
  d.req = data;
  d.ready_cycle = current_cycle + m_config->hbf_latency;
  m_latency_queue.push_back(d);

  m_current_outstanding++;

  // 统计
  if (data->get_is_write()) {
    n_writes++;
    m_total_bytes_written += data->get_data_size();
  } else {
    n_reads++;
    m_total_bytes_read += data->get_data_size();
  }

  if (m_current_outstanding > max_queue_length) {
    max_queue_length = m_current_outstanding;
  }
}

// =============================================================================
// cycle() — 每周期被 memory_partition_unit::hbf_cycle() 调用一次
//
// 只做一件事：扫描 m_latency_queue 头部，把到期的请求搬到 m_returnq。
//
// 延迟机制：
//   push() 时记了 ready_cycle = push_time + hbf_latency。
//   本函数比较 current_cycle >= ready_cycle，成立就代表"HBF 访问完成"。
//
// HBF vs DRAM 延迟对比（都是固定延迟模型）：
//   DRAM: dram_latency = 30 个周期   (~30ns  @ 1GHz)
//   HBF:  hbf_latency  = 10000 个周期 (~10µs @ 1GHz) —— 333x 差距
//
// 返回值：无。完成的请求在 m_returnq 里，由 hbf_cycle() 第一步取走。
// =============================================================================
void hbf_ctrl_t::cycle() {
  unsigned long long current_cycle =
      m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle;

  // 统计：记录队列非空的周期数（反映 HBF 利用率）
  if (m_current_outstanding > 0) {
    n_cycles_queue_nonempty++;
  }

  // 把到期的请求从延迟 FIFO 搬到返回队列
  // 用 while 而非 if —— 同一周期可能有多个请求同时到期
  while (!m_latency_queue.empty() &&
         current_cycle >= m_latency_queue.front().ready_cycle) {
    hbf_delay_t &front = m_latency_queue.front();
    mem_fetch *mf = front.req;

    // set_reply(): 把 READ_REQUEST 翻成 READ_REPLY，WRITE_REQUEST 翻成 WRITE_ACK。
    // 这样 memory_sub_partition 收到后知道这是"回来的数据"而非"出去的请求"。
    // WRBK（写回）类型的请求不需要翻——它们没有 reply 阶段。
    if (mf->get_access_type() != L1_WRBK_ACC &&
        mf->get_access_type() != L2_WRBK_ACC) {
      mf->set_reply();
    }

    // 统计实际延迟（ready_cycle - hbf_latency 就是 push 的时刻）
    unsigned long long latency = current_cycle - (front.ready_cycle -
                                                   m_config->hbf_latency);
    if (mf->get_is_write()) {
      total_write_latency += latency;
    } else {
      total_read_latency += latency;
    }

    // 推入返回队列。
    // 正常情况下 m_returnq 不会满，因为 hbf_cycle() 每周期都会 drain 它。
    if (!m_returnq->full()) {
      m_returnq->push(mf);
    }

    m_latency_queue.pop_front();
    m_current_outstanding--;
  }
}

// =============================================================================
// return_queue_pop / return_queue_top — 返回队列接口
//
// 完全照搬 dram_t 的同名方法。hbf_cycle() 用 top() 偷看队头，
// 用 pop() 确认取走。
// =============================================================================
mem_fetch *hbf_ctrl_t::return_queue_pop() {
  return m_returnq->pop();
}

mem_fetch *hbf_ctrl_t::return_queue_top() {
  return m_returnq->top();
}

unsigned hbf_ctrl_t::que_length() const {
  return m_current_outstanding;
}

// =============================================================================
// print_stat() — 模拟结束后打印 HBF 控制器统计
//
// 输出内容：读/写次数、字节数、平均延迟、最大队列深度、非空周期数。
// 每个 memory partition（共 32 个）各打一份。
// 调用方：memory_partition_unit::print_stat() → m_hbf->print_stat(fp)
// =============================================================================
void hbf_ctrl_t::print_stat(FILE *simFile) {
  fprintf(simFile, "\n========= HBF Controller [%d] Statistics =========\n",
          m_id);
  fprintf(simFile, "HBF Reads:  %llu\n", n_reads);
  fprintf(simFile, "HBF Writes: %llu\n", n_writes);
  fprintf(simFile, "HBF Total Bytes Read:  %llu\n", m_total_bytes_read);
  fprintf(simFile, "HBF Total Bytes Written: %llu\n", m_total_bytes_written);

  if (n_reads > 0) {
    fprintf(simFile, "HBF Avg Read Latency:  %llu cycles\n",
            total_read_latency / n_reads);
  }
  if (n_writes > 0) {
    fprintf(simFile, "HBF Avg Write Latency: %llu cycles\n",
            total_write_latency / n_writes);
  }

  fprintf(simFile, "HBF Max Queue Depth:    %u\n", max_queue_length);
  fprintf(simFile, "HBF Cycles Queue Non-Empty: %llu\n",
          n_cycles_queue_nonempty);

  if (m_current_outstanding > 0) {
    fprintf(simFile, "HBF Outstanding at End: %u\n", m_current_outstanding);
  }
  fprintf(simFile, "=================================================\n\n");
}
