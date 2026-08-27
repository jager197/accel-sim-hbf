// HBF Phase 2: Cycle-Accurate HBF Controller with MSHR Coalescing
//
// Analogous to dram_t (dram.h:112). Replaces Phase 1 hbf_ctrl_t.
//
// Key differences from Phase 1:
//   - NAND sub-array state machines instead of fixed-latency FIFO
//   - MSHR coalescing: multiple 64B cache-line requests → single NAND page read
//   - Power-limited parallelism: max_active subarrays simultaneously
//   - Page-level access: address translated to (subarray, block, page, offset)
//   - v0.4: Shared page cache (LRU) for subarray-read bypass
//
// The controller follows the same interface as dram_t: push(), cycle(),
// return_queue_pop(), return_queue_top(), full().

#ifndef HBF_CONTROLLER_H
#define HBF_CONTROLLER_H

#include <list>
#include <map>
#include <set>
#include <vector>
#include "../abstract_hardware_model.h"
#include "delayqueue.h"
#include "hbf_subarray.h"
#include "hbf_page_cache.h"
#include "hbf_channel.h"

class mem_fetch;
class memory_config;
class memory_partition_unit;
class memory_stats_t;
class gpgpu_sim;
class hbf_ftl_t;

// Channel assignment for a logical 4KiB page (OCP v0.7.0 §11.1.1: data is
// distributed to channels in units of NAND pages). map_mode: 0 = interleave
// (4KiB round-robin across channels), 1 = partition (contiguous per-channel
// regions, OCP §13.3.3).
static inline unsigned hbf_page_to_channel(unsigned num_channels, int map_mode,
                                           unsigned long long page,
                                           unsigned long long pages_per_channel) {
  if (num_channels <= 1) return 0;
  if (map_mode == 1) {
    unsigned ch = (unsigned)(page / pages_per_channel);
    return ch < num_channels ? ch : num_channels - 1;
  }
  return (unsigned)(page % num_channels);
}

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

  // ── Host channels (v0.4, OCP §4.3/§4.5) ─────────────────────────────
  // Each channel owns a fixed slice of the sub-array array (its NAND die
  // set), has its own interface bandwidth credit, and tracks its own
  // outstanding writes. Requests are tagged with their channel at push time
  // and can only be scheduled onto sub-arrays of that channel.
  unsigned m_num_channels;
  hbf_channel_t **m_channels;
  int m_channel_map_mode;          // 0 = interleave, 1 = partition
  unsigned long long m_pages_per_channel;  // for partition mapping

  // Map a 4KiB page to its host channel.
  unsigned channel_of_page(unsigned long long page) const {
    return hbf_page_to_channel(m_num_channels, m_channel_map_mode, page,
                               m_pages_per_channel);
  }

  // ── Scheduling policy (v0.4, case study §6) ─────────────────────────
  // 0 = FCFS (FIFO), 1 = read-priority, 2 = write-drain (read-priority plus
  // bounded write windows opened when the write buffer exceeds the
  // high-water mark). Reads never starve indefinitely: a write window lasts
  // at most hbf_write_drain_maxwait DRAM ticks.
  int m_scheduler;
  unsigned m_write_drain_high;
  unsigned long long m_write_drain_until;  // 0 = no active window
  unsigned long long n_write_drain_windows;
  unsigned long long n_read_deferred_in_window;  // reads held by a window

  // DRAM clock frequency in MHz (the domain hbf_cycle runs in; all NAND
  // timing counters count DRAM-clock ticks). Used only for statistics
  // reporting (ticks -> us) and documentation.
  double m_dram_freq_mhz;
  // Core clock frequency in MHz (end-to-end latencies are timestamped in
  // core cycles via gpu_sim_cycle). Used only for statistics reporting.
  double m_core_freq_mhz;

  // Active sub-array set — only these are cycled each tick. Without this,
  // cycling/scanning all 16K sub-arrays every cycle dominates runtime
  // (~1.5K cycles/sec); tracking only the active ones gives >10x speedup.
  std::set<unsigned> m_active_subarrays;

  // MSHR: Miss Status Holding Register
  // Multiple 64B requests to the same 4KB page are coalesced into
  // a single NAND page read. This is the key mechanism that enables
  // HBF to match HBM bandwidth despite µs-scale NAND latency.
  // MSHR operation state (for erase-before-write flow)
  enum mshr_op_t { OP_WAITING, OP_READING, OP_ERASING, OP_PROGRAMMING };

  struct mshr_entry_t {
    unsigned long long phys_page;       // physical page address
    unsigned channel_id;               // owning host channel (v0.4)
    unsigned subarray_id;              // which sub-array (if in_flight)
    unsigned block_id;                 // which block
    unsigned page_offset;             // offset within block
    mshr_op_t op_state;              // current operation: WAITING/ERASING/PROGRAMMING/READING
    bool needs_erase;                // true if block must be erased before programming
    bool delivery_pending;           // op finished; replies waiting for returnq room
    bool fill_cache_on_deliver;      // finished op was a READ → fill page cache on delivery
    unsigned long long issue_cycle;   // when operation was issued to sub-array
    unsigned long long cache_hit_ready_cycle;  // 0 = no cache hit; >0 = ready at this cycle
    std::vector<mem_fetch *> pending; // requests waiting for this page
  };
  std::map<unsigned long long, mshr_entry_t> m_mshr;
  std::list<unsigned long long> m_mshr_queue;  // FIFO of pages waiting for sub-array

  // Return queue (completed requests)
  fifo_pipeline<mem_fetch> *m_returnq;

  // Simple FTL (page-level mapping)
  hbf_ftl_t *m_ftl;

  // v0.4: Shared page cache (LRU, logic-die SRAM)
  hbf_page_cache_t *m_page_cache;

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
  void flush_write_buffer_entry(unsigned long long page_addr, bool force);

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
  unsigned long long n_read_requests;   // completed read requests (latency stats)
  unsigned long long max_read_latency;  // max end-to-end read latency (core cycles)
  unsigned long long n_gc_stall_cycles;  // accumulated cycles lost to GC
  unsigned long long n_cache_hits;       // page cache hits
  unsigned long long n_cache_misses;     // page cache misses
  // v0.4 write-path statistics (OCP §5.4.1):
  unsigned long long n_partial_page_programs; // programs with < full 4KiB page data
  unsigned long long n_write_timeout_flushes; // flushes caused by aggregation timeout
  unsigned long long n_idle_drain_flushes;    // flushes caused by device-idle drain
};

#endif  // HBF_CONTROLLER_H
