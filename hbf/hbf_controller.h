// HBF Phase 2: Cycle-Accurate HBF Controller with MSHR Coalescing
//
// Analogous to dram_t (dram.h:112). Replaces Phase 1 hbf_ctrl_t.
//
// Key differences from Phase 1:
//   - NAND sub-array state machines instead of fixed-latency FIFO
//   - MSHR coalescing: multiple 64B cache-line requests → single NAND page read
//   - Power-limited parallelism: max_active subarrays simultaneously
//   - Page-level access: address translated to (subarray, block, page, offset)
//   - logic-die page cache (LRU) for subarray-read bypass
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
#include "hbf_mapping.h"
#include "hbf_trace.h"

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
    if (pages_per_channel == 0) return num_channels - 1;
    unsigned ch = (unsigned)(page / pages_per_channel);
    return ch < num_channels ? ch : num_channels - 1;
  }
  return (unsigned)(page % num_channels);
}

static inline bool hbf_direct_page_identity(unsigned long long logical_page,
                                            unsigned pages_per_block,
                                            unsigned *block,
                                            unsigned *page_offset) {
  const unsigned direct_block_bit = 1u << 31;
  if (pages_per_block == 0) pages_per_block = 1;
  unsigned long long direct_block = logical_page / pages_per_block;
  if (direct_block >= direct_block_bit) return false;
  *block = direct_block_bit | (unsigned)direct_block;
  *page_offset = (unsigned)(logical_page % pages_per_block);
  return true;
}

static inline unsigned hbf_delivery_batch_limit(unsigned pending,
                                                unsigned queue_length,
                                                unsigned queue_capacity) {
  if (queue_length >= queue_capacity) return 0;
  unsigned slots = queue_capacity - queue_length;
  return pending < slots ? pending : slots;
}

static inline unsigned hbf_write_padding_bytes(unsigned page_size,
                                               unsigned covered_bytes) {
  return covered_bytes < page_size ? page_size - covered_bytes : 0;
}

static inline unsigned hbf_effective_write_bytes(
    const mem_access_byte_mask_t &mask, unsigned fallback_bytes) {
  return mask.any() ? (unsigned)mask.count() : fallback_bytes;
}

struct hbf_schedule_candidate_t {
  bool waiting;
  bool aggregation_pending;
  bool subarray_idle;
  bool is_read;
};

// Return the first schedulable request of the preferred type, then the first
// schedulable request of either type. Busy-subarray entries are deliberately
// skipped so one queue head cannot block independent subarrays.
static inline int hbf_pick_schedule_candidate(
    const std::vector<hbf_schedule_candidate_t> &candidates,
    bool prefer_reads, bool use_type_preference) {
  unsigned passes = use_type_preference ? 2 : 1;
  for (unsigned pass = 0; pass < passes; ++pass) {
    for (size_t i = 0; i < candidates.size(); ++i) {
      const hbf_schedule_candidate_t &candidate = candidates[i];
      if (!candidate.waiting || candidate.aggregation_pending ||
          !candidate.subarray_idle)
        continue;
      if (!use_type_preference || pass == 1 ||
          candidate.is_read == prefer_reads)
        return (int)i;
    }
  }
  return -1;
}

// Count schedulable reads that lost this issue opportunity to a write during
// an active write-drain window. This is a pressure count, not a unique-request
// count: a read can be deferred by more than one issued write.
static inline unsigned hbf_count_deferred_reads(
    const std::vector<hbf_schedule_candidate_t> &candidates, int selected,
    bool drain_active) {
  if (!drain_active || selected < 0 ||
      (size_t)selected >= candidates.size() || candidates[selected].is_read)
    return 0;
  unsigned deferred = 0;
  for (const hbf_schedule_candidate_t &candidate : candidates) {
    if (candidate.waiting && !candidate.aggregation_pending &&
        candidate.subarray_idle && candidate.is_read)
      ++deferred;
  }
  return deferred;
}

enum hbf_write_drain_event_t {
  HBF_DRAIN_UNCHANGED,
  HBF_DRAIN_OPENED,
  HBF_DRAIN_EXPIRED,
  HBF_DRAIN_RELEASED,
};

// Advance the bounded write-drain state. After a window expires under
// sustained pressure, read_due blocks an immediate reopen until the scheduler
// issues one read.
static inline hbf_write_drain_event_t hbf_update_write_drain(
    unsigned pending_writes, unsigned high_watermark,
    unsigned long long current_cycle, unsigned maxwait,
    unsigned long long *drain_until, bool *read_due) {
  if (drain_until == NULL || read_due == NULL)
    return HBF_DRAIN_UNCHANGED;
  if (high_watermark == 0) high_watermark = 1;
  if (pending_writes < high_watermark) {
    bool changed = *drain_until != 0 || *read_due;
    *drain_until = 0;
    *read_due = false;
    return changed ? HBF_DRAIN_RELEASED : HBF_DRAIN_UNCHANGED;
  }
  if (*drain_until != 0) {
    if (current_cycle >= *drain_until) {
      *drain_until = 0;
      *read_due = true;
      return HBF_DRAIN_EXPIRED;
    }
    return HBF_DRAIN_UNCHANGED;
  }
  if (*read_due) return HBF_DRAIN_UNCHANGED;
  *drain_until = (~0ull - current_cycle < maxwait)
                     ? ~0ull
                     : current_cycle + maxwait;
  return HBF_DRAIN_OPENED;
}

// Exact byte coverage for one NAND page aggregation window. Keeping this
// independent of mem_fetch makes the core accounting directly unit-testable.
class hbf_write_coverage_t {
 public:
  hbf_write_coverage_t() : m_covered_bytes(0) {}
  explicit hbf_write_coverage_t(unsigned page_size)
      : m_covered(page_size, 0), m_covered_bytes(0) {}

  void reset(unsigned page_size) {
    m_covered.assign(page_size, 0);
    m_covered_bytes = 0;
  }

  // Returns false for a request that is not wholly contained in this page.
  // new_bytes receives only bytes not covered by an earlier request.
  bool add(unsigned long long page_offset, unsigned bytes,
           unsigned *new_bytes) {
    if (new_bytes != NULL) *new_bytes = 0;
    if (page_offset > m_covered.size() ||
        bytes > m_covered.size() - page_offset) {
      return false;
    }
    unsigned added = 0;
    for (unsigned long long i = page_offset;
         i < page_offset + bytes; ++i) {
      if (m_covered[(size_t)i] == 0) {
        m_covered[(size_t)i] = 1;
        added++;
      }
    }
    m_covered_bytes += added;
    if (new_bytes != NULL) *new_bytes = added;
    return true;
  }

  // Byte-mask bits are relative to the enclosing 128-byte memory-access
  // window, not necessarily to mem_fetch::get_addr(). Validate the complete
  // mask before mutating coverage so a cross-page request is atomic.
  bool add_mask(unsigned long long mask_page_offset,
                const mem_access_byte_mask_t &mask, unsigned *new_bytes) {
    if (new_bytes != NULL) *new_bytes = 0;
    for (unsigned i = 0; i < MAX_MEMORY_ACCESS_SIZE; ++i) {
      if (mask.test(i) && mask_page_offset + i >= m_covered.size())
        return false;
    }
    unsigned added = 0;
    for (unsigned i = 0; i < MAX_MEMORY_ACCESS_SIZE; ++i) {
      if (!mask.test(i)) continue;
      size_t offset = (size_t)(mask_page_offset + i);
      if (m_covered[offset] == 0) {
        m_covered[offset] = 1;
        ++added;
      }
    }
    m_covered_bytes += added;
    if (new_bytes != NULL) *new_bytes = added;
    return true;
  }

  unsigned covered_bytes() const { return m_covered_bytes; }
  unsigned page_size() const { return (unsigned)m_covered.size(); }
  bool full() const {
    return !m_covered.empty() && m_covered_bytes == m_covered.size();
  }

 private:
  std::vector<unsigned char> m_covered;
  unsigned m_covered_bytes;
};

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
  bool has_work() const;
  unsigned get_num_channels() const { return m_num_channels; }

  // Accessors
  unsigned get_id() const { return m_id; }

 private:
  // Translate a 64B cache-line address to a NAND page address
  unsigned long long addr_to_page(new_addr_type addr) const;
  // Schedule: assign MSHR entries to idle sub-arrays
  void schedule_operations();
  hbf_subarray_t *ensure_subarray(unsigned subarray_id);

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
  bool m_explicit_placement;

  // Map a 4KiB page to its host channel.
  unsigned channel_of_page(unsigned long long page) const;
  unsigned subarray_of_page(unsigned long long page, unsigned channel) const;
  void direct_physical_address(unsigned long long page, unsigned channel,
                               unsigned *subarray, unsigned *block,
                               unsigned *page_offset) const;
  [[noreturn]] void fail_fast(const char *reason,
                              unsigned long long page) const;

  // ── Scheduling policy (v0.4, case study §6) ─────────────────────────
  // 0 = FCFS (FIFO), 1 = read-priority, 2 = write-drain (read-priority plus
  // bounded write windows opened when the write buffer exceeds the
  // high-water mark). Reads never starve indefinitely: a write window lasts
  // at most hbf_write_drain_maxwait DRAM ticks.
  int m_scheduler;
  unsigned m_write_drain_high;
  unsigned long long m_write_drain_until;  // 0 = no active window
  bool m_write_drain_read_due;
  unsigned long long n_write_drain_windows;
  unsigned long long n_read_deferred_in_window;  // read issue opportunities held
  unsigned long long n_write_drain_read_breaks;

  // DRAM clock frequency in MHz (the domain hbf_cycle runs in; all NAND
  // timing counters count DRAM-clock ticks). Used only for statistics
  // reporting (ticks -> us) and documentation.
  double m_dram_freq_mhz;
  // Core clock frequency in MHz (end-to-end latencies are timestamped in
  // core cycles via gpu_sim_cycle). Used only for statistics reporting.
  double m_core_freq_mhz;
  unsigned long long m_hbf_tick;

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
    bool delivery_pending;           // op finished; replies waiting for returnq room
    bool fill_cache_on_deliver;      // finished op was a READ → fill page cache on delivery
    bool read_aggregation_pending;   // aggregation mode: wait for window/threshold
    bool is_write_operation;         // retained while replies are batch-delivered
    bool first_delivery_recorded;    // page-level delivery stats count once
    unsigned long long issue_cycle;   // when operation was issued to sub-array
    unsigned long long first_arrival_cycle;
    unsigned long long first_arrival_tick;
    unsigned long long requested_bytes;
    unsigned write_covered_bytes;    // unique bytes in this write aggregation window
    unsigned long long cache_hit_ready_tick;  // 0 = no cache hit; >0 = ready at this HBF tick
    std::vector<mem_fetch *> pending; // requests waiting for this page
  };
  std::map<unsigned long long, mshr_entry_t> m_mshr;
  std::list<unsigned long long> m_mshr_queue;  // FIFO of pages waiting for sub-array

  // Return queue (completed requests)
  fifo_pipeline<mem_fetch> *m_returnq;

  // Simple FTL (page-level mapping)
  hbf_ftl_t *m_ftl;

  // Logic-die page cache (LRU SRAM), distinct from CUDA Shared Memory.
  hbf_page_cache_t *m_page_cache;

  // Strict explicit page-to-channel placement table (placement mode 2).
  std::map<unsigned long long, unsigned> m_explicit_channel_map;

  // Write buffer: coalesce writes to same page before creating MSHR entry.
  // Hides tPROG latency by batching writes to the same page.
  struct write_buffer_entry_t {
    unsigned long long page_addr;
    std::vector<mem_fetch *> requests;
    unsigned long long first_arrival_cycle;
    unsigned long long first_arrival_tick;
    unsigned long long last_arrival_tick;
    hbf_write_coverage_t coverage;
  };
  std::map<unsigned long long, write_buffer_entry_t> m_write_buffer;
  unsigned m_write_buffer_max;  // max entries before forced flush

  int m_read_mode;              // 0=demand, 1=aggregation
  unsigned m_read_agg_window;   // DRAM ticks
  unsigned m_read_agg_threshold; // bytes
  int m_write_timeout_policy;   // 0=partial, 1=strict fail-fast
  hbf_trace_t *m_trace;

  unsigned flush_write_buffer();
  bool flush_write_buffer_entry(unsigned long long page_addr, bool force);
  bool add_write_coverage(write_buffer_entry_t *entry, const mem_fetch *data);

  // Statistics
  unsigned long long n_page_reads;
  unsigned long long n_page_programs;
  unsigned long long n_block_erases;
  unsigned long long n_mshr_hits;      // requests coalesced into existing MSHR entry
  unsigned long long n_total_requests; // total requests received
  // Passive diagnostics: queued read request-ticks, sampled before media advance.
  unsigned long long n_diag_read_wait_ticks;
  unsigned long long n_diag_read_program_wait_ticks;
  unsigned long long total_read_latency;
  unsigned long long total_write_latency;
  unsigned max_queue_depth;
  unsigned long long n_cycles_active;
  unsigned long long n_bytes_read;
  unsigned long long n_bytes_written;
  unsigned long long n_read_requests;   // completed read requests (latency stats)
  unsigned long long n_write_requests;  // completed write requests (latency stats)
  unsigned long long max_read_latency;  // max end-to-end read latency (core cycles)
  unsigned long long n_gc_stall_cycles;  // accumulated cycles lost to GC
  unsigned long long n_cache_hits;       // page cache hits
  unsigned long long n_cache_misses;     // page cache misses
  // v0.4 write-path statistics (OCP §5.4.1):
  unsigned long long n_partial_page_programs; // programs with < full 4KiB page data
  unsigned long long n_write_timeout_flushes; // flushes caused by aggregation timeout
  unsigned long long n_idle_drain_flushes;    // flushes caused by device-idle drain
  unsigned long long n_incomplete_page_errors;
  unsigned long long n_write_coverage_errors;
  unsigned long long n_mapping_errors;
  unsigned long long n_host_write_bytes;
  unsigned long long n_unique_write_bytes;
  unsigned long long n_media_program_bytes;
  unsigned long long n_padding_bytes;
  unsigned long long n_requested_read_bytes;
  unsigned long long n_media_read_bytes;
  unsigned long long n_read_aggregation_wait;
  unsigned long long n_read_aggregation_pages;
  unsigned long long n_first_request_latency;
  unsigned long long n_first_request_pages;
};

#endif  // HBF_CONTROLLER_H
