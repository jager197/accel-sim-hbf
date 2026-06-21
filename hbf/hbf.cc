// HBF (High Bandwidth Flash) Simulation Module — Phase 1 Implementation
//
// Fixed-latency HBF controller. Models HBF as a configurable-latency memory
// tier. Incoming requests are pushed into a latency FIFO and emerge after
// hbf_latency cycles. This captures the first-order effect of HBF: higher
// latency than DRAM but comparable bandwidth (configurable through queue depth
// and outstanding request limit).

#include "hbf.h"
#include <stdio.h>
#include "../abstract_hardware_model.h"
#include "gpu-sim.h"
#include "l2cache.h"
#include "mem_fetch.h"
#include "mem_latency_stat.h"

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
  // Initialize statistics counters
  n_reads = 0;
  n_writes = 0;
  total_read_latency = 0;
  total_write_latency = 0;
  max_queue_length = 0;
  n_cycles_queue_nonempty = 0;

  // Create return queue with latency equal to 0 (return path is immediate
  // once the request completes its hbf_latency wait)
  m_returnq = new fifo_pipeline<mem_fetch>("hbf_returnq", 0,
                                            config->gpgpu_dram_return_queue_size);
}

bool hbf_ctrl_t::full(bool is_write) const {
  // Check if the latency queue has reached its maximum size
  unsigned int max_q = m_config->hbf_max_outstanding;
  if (max_q == 0) return false;  // 0 means unlimited
  return m_current_outstanding >= max_q;
}

void hbf_ctrl_t::push(mem_fetch *data) {
  unsigned long long current_cycle =
      m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle;

  hbf_delay_t d;
  d.req = data;
  d.ready_cycle = current_cycle + m_config->hbf_latency;
  m_latency_queue.push_back(d);

  m_current_outstanding++;

  if (data->get_is_write()) {
    n_writes++;
    m_total_bytes_written += data->get_data_size();
  } else {
    n_reads++;
    m_total_bytes_read += data->get_data_size();
  }

  // Track queue depth statistics
  if (m_current_outstanding > max_queue_length) {
    max_queue_length = m_current_outstanding;
  }
}

void hbf_ctrl_t::cycle() {
  unsigned long long current_cycle =
      m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle;

  // Track cycles where queue is non-empty
  if (m_current_outstanding > 0) {
    n_cycles_queue_nonempty++;
  }

  // Process completed requests: move from latency queue to return queue
  while (!m_latency_queue.empty() &&
         current_cycle >= m_latency_queue.front().ready_cycle) {
    hbf_delay_t &front = m_latency_queue.front();
    mem_fetch *mf = front.req;

    // Set reply status (converts READ_REQUEST→READ_REPLY, WRITE_REQUEST→WRITE_ACK)
    if (mf->get_access_type() != L1_WRBK_ACC &&
        mf->get_access_type() != L2_WRBK_ACC) {
      mf->set_reply();
    }

    // Track latency statistics
    unsigned long long latency = current_cycle - (front.ready_cycle -
                                                   m_config->hbf_latency);
    if (mf->get_is_write()) {
      total_write_latency += latency;
    } else {
      total_read_latency += latency;
    }

    // Push to return queue
    if (!m_returnq->full()) {
      m_returnq->push(mf);
    }
    // Note: if return queue is full, request stays in latency queue until
    // return queue has space. In practice this shouldn't happen because
    // the memory_partition_unit drains the return queue each cycle.

    m_latency_queue.pop_front();
    m_current_outstanding--;
  }
}

mem_fetch *hbf_ctrl_t::return_queue_pop() {
  mem_fetch *mf = m_returnq->pop();
  return mf;
}

mem_fetch *hbf_ctrl_t::return_queue_top() {
  mem_fetch *mf = m_returnq->top();
  return mf;
}

unsigned hbf_ctrl_t::que_length() const {
  return m_current_outstanding;
}

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
