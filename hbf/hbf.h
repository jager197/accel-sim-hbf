// HBF (High Bandwidth Flash) Simulation Module — Phase 1: Fixed-Latency Controller
//
// HBF is a new memory technology that stacks 3D NAND dies via TSV onto a logic die
// on the GPU interposer, providing HBM-comparable bandwidth with 8-16x capacity.
//
// Phase 1 models HBF as a fixed-latency memory tier alongside DRAM, following the
// simple_dram_model pattern. Requests are queued with a configurable latency and
// returned to the L2 cache through the existing return path.
//
// Copyright (c) 2024-2025, HBF Simulator Project
// Based on the GPGPU-Sim dram_t pattern (dram.h)

#ifndef HBF_H
#define HBF_H

#include <list>
#include "delayqueue.h"

class mem_fetch;
class memory_config;
class memory_partition_unit;
class memory_stats_t;
class gpgpu_sim;

// Lightweight delay entry for HBF latency queue
struct hbf_delay_t {
  unsigned long long ready_cycle;
  class mem_fetch *req;
};

// Fixed-latency HBF controller (Phase 1).
//
// Accepts memory requests from memory_partition_unit, holds them in a FIFO
// latency queue for hbf_latency cycles, then returns completed requests
// through the return queue. Tracks basic statistics.
//
// Follows the same interface pattern as dram_t to enable drop-in integration.
class hbf_ctrl_t {
 public:
  hbf_ctrl_t(unsigned int partition_id, const memory_config *config,
             class memory_stats_t *stats, class memory_partition_unit *mp,
             class gpgpu_sim *gpu);

  // Check if the controller can accept more requests
  bool full(bool is_write) const;

  // Push a new memory request into the HBF latency queue
  void push(class mem_fetch *data);

  // Process one cycle: move ready requests from latency queue to return queue
  void cycle();

  // Interface matching dram_t for return-path integration
  class mem_fetch *return_queue_pop();
  class mem_fetch *return_queue_top();

  // Queue depth for arbitration
  unsigned que_length() const;

  // Print statistics
  void print_stat(FILE *simFile);

  // Accessors
  unsigned int get_id() const { return m_id; }

  // Statistics
  unsigned long long n_reads;
  unsigned long long n_writes;
  unsigned long long total_read_latency;
  unsigned long long total_write_latency;
  unsigned int max_queue_length;
  unsigned long long n_cycles_queue_nonempty;

 private:
  unsigned int m_id;
  const memory_config *m_config;
  class memory_stats_t *m_stats;
  class memory_partition_unit *m_memory_partition_unit;
  class gpgpu_sim *m_gpu;

  // Pipeline FIFOs (same pattern as dram_t)
  fifo_pipeline<mem_fetch> *m_returnq;

  // Latency queue: requests wait here for hbf_latency cycles
  std::list<hbf_delay_t> m_latency_queue;

  // Current outstanding request count
  unsigned int m_current_outstanding;

  // Bandwidth tracking
  unsigned long long m_total_bytes_read;
  unsigned long long m_total_bytes_written;
};

#endif  // HBF_H
