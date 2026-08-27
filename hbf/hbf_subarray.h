// HBF Phase 2: NAND Sub-array State Machine
//
// Analogous to bank_t (dram.h:78). Models one NAND flash die's behavior:
// - IDLE → READING → IDLE (page read: ~15us)
// - IDLE → PROGRAMMING → IDLE (page program: ~200us)
// - IDLE → ERASING → IDLE (block erase: ~2ms)
//
// Unlike DRAM banks which have open-row states and complex timing constraints,
// NAND sub-arrays are simpler: one operation at a time, one timing counter.
//
// v0.3: Models the NAND page register — a per-subarray 1-page buffer that
// holds the most recently accessed page. Re-reading the same page hits the
// buffer (hbf_buffer_hit_latency cycles) instead of paying full tR.
//
// v0.4: OCP §5.3.1.7 requires TWO cache buffers per bank (each holding at
// least one page). The per-sub-array register becomes a small buffer array
// sized by -gpgpu_hbf_page_buffers (default 2).

#ifndef HBF_SUBARRAY_H
#define HBF_SUBARRAY_H

#include <vector>

class memory_config;

class hbf_subarray_t {
 public:
  enum state_t {
    IDLE = 0,
    READING,
    PROGRAMMING,
    ERASING
  };

  hbf_subarray_t(unsigned id, const memory_config *config);

  // Check if sub-array can accept a new operation
  bool is_idle() const { return m_state == IDLE; }

  // Start operations (only valid when IDLE)
  void start_read(unsigned page, unsigned block);
  void start_program(unsigned page, unsigned block);
  void start_erase(unsigned block);

  // Per-cycle: decrement timing counter, transition IDLE when done
  void cycle();

  // Accessors
  state_t get_state() const { return m_state; }
  unsigned get_id() const { return m_id; }
  unsigned get_curr_page() const { return m_curr_page; }
  unsigned get_curr_block() const { return m_curr_block; }

  // Statistics
  unsigned long long n_reads;
  unsigned long long n_writes;
  unsigned long long n_erases;
  unsigned pe_cycles;  // program/erase cycle count (wear tracking)

  // Page buffer statistics
  unsigned long long n_buffer_hits;
  unsigned long long n_buffer_misses;

 private:
  unsigned m_id;
  const memory_config *m_config;

  state_t m_state;
  unsigned m_curr_page;
  unsigned m_curr_block;

  // Cycles remaining in current operation (decremented each cycle())
  unsigned m_timing_remaining;

  // NAND timing parameters (from config, in DRAM-clock ticks)
  unsigned m_tR;      // page read latency
  unsigned m_tPROG;   // page program latency
  unsigned m_tBERS;   // block erase latency
  unsigned m_tBUFF;   // page buffer hit latency

  // Page buffers (NAND page registers): each holds one page. A re-read of a
  // buffered page need not pay full tR (OCP §5.3.1.7: >= 2 buffers per bank;
  // cache-hit reads are served immediately if ordering is not violated).
  // Disable via config (hbf_buffer_enabled 0) to model a slow storage with
  // no page register (e.g. external spill memory).
  struct hbf_page_buffer_t {
    bool valid;
    unsigned page;
    unsigned block;
  };
  bool m_buffer_enabled;
  std::vector<hbf_page_buffer_t> m_buffers;  // size = hbf_page_buffers
  unsigned m_buffer_next;  // round-robin slot for replacement/insertion
};

#endif  // HBF_SUBARRAY_H
