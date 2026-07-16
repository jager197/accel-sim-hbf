// HBF Phase 2: NAND Sub-array State Machine Implementation
//
// v0.3: Models the per-subarray page buffer (NAND page register).
// A re-read of the same page on the same subarray hits the buffer
// and pays hbf_buffer_hit_latency instead of full tR.

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
  m_tBUFF = config->hbf_buffer_hit_latency;

  // Page buffer starts empty
  m_buffer_valid = false;
  m_buffer_page = 0;
  m_buffer_block = 0;

  n_reads   = 0;
  n_writes  = 0;
  n_erases  = 0;
  pe_cycles = 0;

  n_buffer_hits   = 0;
  n_buffer_misses = 0;
}

void hbf_subarray_t::start_read(unsigned page, unsigned block) {
  m_state      = READING;
  m_curr_page  = page;
  m_curr_block = block;

  // Page buffer check: same page already in the register?
  if (m_buffer_valid && m_buffer_page == page && m_buffer_block == block) {
    // Buffer hit — data already in page register, minimal latency
    m_timing_remaining = m_tBUFF;
    n_buffer_hits++;
  } else {
    // Buffer miss — full NAND cell read required
    m_timing_remaining = m_tR;
    n_buffer_misses++;
  }

  // Page register now holds this page (loading it is part of the read)
  m_buffer_valid = true;
  m_buffer_page  = page;
  m_buffer_block = block;

  n_reads++;
}

void hbf_subarray_t::start_program(unsigned page, unsigned block) {
  m_state      = PROGRAMMING;
  m_curr_page  = page;
  m_curr_block = block;

  m_timing_remaining = m_tPROG;

  // Program loads data into page register before writing to NAND cells,
  // so the buffer now holds this page.
  m_buffer_valid = true;
  m_buffer_page  = page;
  m_buffer_block = block;

  n_writes++;
  pe_cycles++;
}

void hbf_subarray_t::start_erase(unsigned block) {
  m_state      = ERASING;
  m_curr_block = block;

  m_timing_remaining = m_tBERS;

  // Erasing a block invalidates all its pages in the page register.
  if (m_buffer_valid && m_buffer_block == block) {
    m_buffer_valid = false;
  }

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
