// HBF Phase 2: Cycle-Accurate HBF Controller with MSHR Coalescing
//
// Analogous to dram_t (dram.h:112). Replaces Phase 1 hbf_ctrl_t.
//
// Key differences from Phase 1:
//   - NAND sub-array state machines instead of fixed-latency FIFO
//   - MSHR coalescing: multiple 64B cache-line requests → single NAND page read
//   - Power-limited parallelism: max_active subarrays simultaneously
//   - Page-level access: address translated to (subarray, block, page, offset)
//
// The controller follows the same interface as dram_t: push(), cycle(),
// return_queue_pop(), return_queue_top(), full().

#ifndef HBF_CONTROLLER_H
#define HBF_CONTROLLER_H

#include <list>
#include <map>
#include <vector>
#include "../abstract_hardware_model.h"
#include "delayqueue.h"
#include "hbf_subarray.h"

class mem_fetch;
class memory_config;
class memory_partition_unit;
class memory_stats_t;
class gpgpu_sim;
class hbf_ftl_t;

class hbf_controller_t {
 public:
  hbf_controller_t(unsigned partition_id, const memory_config *config,
                   memory_stats_t *stats, memory_partition_unit *mp,
                   gpgpu_sim *gpu);
  ~hbf_controller_t();

  // Standard controller interface (matches dram_t)
  bool full(bool is_write) const;
  void push(mem_fetch *data);
  void cycle();
  mem_fetch *return_queue_pop();
  mem_fetch *return_queue_top();
  unsigned que_length() const;
  void print_stat(FILE *simFile);

  // Accessors
  unsigned get_id() const { return m_id; }

 private:
  // Translate a 64B cache-line address to a NAND page address
  unsigned long long addr_to_page(new_addr_type addr) const;
  // Schedule: assign MSHR entries to idle sub-arrays
  void schedule_operations();

  unsigned m_id;
  const memory_config *m_config;
  memory_stats_t *m_stats;
  memory_partition_unit *m_memory_partition_unit;
  gpgpu_sim *m_gpu;

  // Sub-array array (thousands of NAND dies, independently operable)
  hbf_subarray_t **m_subarrays;
  unsigned m_num_subarrays;
  unsigned m_max_active;  // power-limited max concurrent operations

  // MSHR: Miss Status Holding Register
  // Multiple 64B requests to the same 4KB page are coalesced into
  // a single NAND page read. This is the key mechanism that enables
  // HBF to match HBM bandwidth despite µs-scale NAND latency.
  // MSHR operation state (for erase-before-write flow)
  enum mshr_op_t { OP_WAITING, OP_READING, OP_ERASING, OP_PROGRAMMING };

  struct mshr_entry_t {
    unsigned long long phys_page;       // physical page address
    unsigned subarray_id;              // which sub-array (if in_flight)
    unsigned block_id;                 // which block
    unsigned page_offset;             // offset within block
    mshr_op_t op_state;              // current operation: WAITING/ERASING/PROGRAMMING/READING
    bool needs_erase;                // true if block must be erased before programming
    unsigned long long issue_cycle;   // when operation was issued to sub-array
    std::vector<mem_fetch *> pending; // requests waiting for this page
  };
  std::map<unsigned long long, mshr_entry_t> m_mshr;
  std::list<unsigned long long> m_mshr_queue;  // FIFO of pages waiting for sub-array

  // Return queue (completed requests)
  fifo_pipeline<mem_fetch> *m_returnq;

  // Simple FTL (page-level mapping)
  hbf_ftl_t *m_ftl;

  // Write buffer: coalesce writes to same page before creating MSHR entry.
  // Hides tPROG latency by batching writes to the same page.
  struct write_buffer_entry_t {
    unsigned long long page_addr;
    std::vector<mem_fetch *> requests;
    unsigned long long first_arrival;
  };
  std::map<unsigned long long, write_buffer_entry_t> m_write_buffer;
  unsigned m_write_buffer_max;  // max entries before forced flush

  void flush_write_buffer();
  void flush_write_buffer_entry(unsigned long long page_addr);

  // Statistics
  unsigned long long n_page_reads;
  unsigned long long n_page_programs;
  unsigned long long n_block_erases;
  unsigned long long n_mshr_hits;      // requests coalesced into existing MSHR entry
  unsigned long long n_total_requests; // total requests received
  unsigned long long total_read_latency;
  unsigned long long total_write_latency;
  unsigned max_queue_depth;
  unsigned long long n_cycles_active;
  unsigned long long n_bytes_read;
  unsigned long long n_bytes_written;
};

#endif  // HBF_CONTROLLER_H
