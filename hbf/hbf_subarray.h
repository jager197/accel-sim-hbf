// HBF Phase 2: NAND Sub-array State Machine
//
// Analogous to bank_t (dram.h:78). Models one NAND flash die's behavior:
// - IDLE → READING → IDLE (page read: ~15us)
// - IDLE → PROGRAMMING → IDLE (page program: ~200us)
// - IDLE → ERASING → IDLE (block erase: ~2ms)
//
// Unlike DRAM banks which have open-row states and complex timing constraints,
// NAND sub-arrays are simpler: one operation at a time, one timing counter.

#ifndef HBF_SUBARRAY_H
#define HBF_SUBARRAY_H

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

 private:
  unsigned m_id;
  const memory_config *m_config;

  state_t m_state;
  unsigned m_curr_page;
  unsigned m_curr_block;

  // Cycles remaining in current operation (decremented each cycle())
  unsigned m_timing_remaining;

  // NAND timing parameters (from config, in GPU core cycles)
  unsigned m_tR;      // page read latency
  unsigned m_tPROG;   // page program latency
  unsigned m_tBERS;   // block erase latency
};

#endif  // HBF_SUBARRAY_H
