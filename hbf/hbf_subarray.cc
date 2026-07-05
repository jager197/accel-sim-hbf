// HBF Phase 2: NAND Sub-array State Machine Implementation

#include "hbf_subarray.h"
#include "gpu-sim.h"

hbf_subarray_t::hbf_subarray_t(unsigned id, const memory_config *config)
    : m_id(id),
      m_config(config),
      m_state(IDLE),
      m_curr_page(0),
      m_curr_block(0),
      m_timing_remaining(0) {
  // Load NAND timing from config
  m_tR    = config->hbf_tR;
  m_tPROG = config->hbf_tPROG;
  m_tBERS = config->hbf_tBERS;

  n_reads   = 0;
  n_writes  = 0;
  n_erases  = 0;
  pe_cycles = 0;
}

void hbf_subarray_t::start_read(unsigned page, unsigned block) {
  m_state       = READING;
  m_curr_page   = page;
  m_curr_block  = block;
  m_timing_remaining = m_tR;
  n_reads++;
}

void hbf_subarray_t::start_program(unsigned page, unsigned block) {
  m_state       = PROGRAMMING;
  m_curr_page   = page;
  m_curr_block  = block;
  m_timing_remaining = m_tPROG;
  n_writes++;
  pe_cycles++;
}

void hbf_subarray_t::start_erase(unsigned block) {
  m_state       = ERASING;
  m_curr_block  = block;
  m_timing_remaining = m_tBERS;
  n_erases++;
  pe_cycles++;
}

void hbf_subarray_t::cycle() {
  if (m_state == IDLE) return;

  // Decrement timing counter. When it reaches 0, operation is complete.
  if (m_timing_remaining > 0) {
    m_timing_remaining--;
  }
  if (m_timing_remaining == 0) {
    m_state = IDLE;
  }
}
