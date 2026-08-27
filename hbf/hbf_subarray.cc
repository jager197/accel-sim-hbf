// HBF Phase 2: NAND Sub-array State Machine Implementation
//
// v0.3: Models the per-subarray page buffer (NAND page register).
// A re-read of the same page on the same subarray hits the buffer
// and pays hbf_buffer_hit_latency instead of full tR.
//
// v0.4: OCP §5.3.1.7 — each bank has >= 2 page buffers; the single
// register becomes a buffer array sized by -gpgpu_hbf_page_buffers.

#include "hbf_subarray.h"
#include "gpu-sim.h"

hbf_subarray_t::hbf_subarray_t(unsigned id, const memory_config *config)
    : m_id(id),
      m_config(config),
      m_state(IDLE),
      m_curr_page(0),
      m_curr_block(0),
      m_timing_remaining(0) {
  // Load NAND timing from config (units: DRAM-clock ticks, see
  // docs/validation.md for the clock-domain rationale).
  m_tR    = config->hbf_tR;
  m_tPROG = config->hbf_tPROG;
  m_tBERS = config->hbf_tBERS;
  m_tBUFF = config->hbf_buffer_hit_latency;
  m_buffer_enabled = config->hbf_buffer_enabled;

  // Page buffers start empty. OCP §5.3.1.7: at least two buffers per bank.
  unsigned nb = config->hbf_page_buffers;
  if (nb == 0) nb = 2;
  m_buffers.resize(nb);
  for (auto &b : m_buffers) {
    b.valid = false;
    b.page = 0;
    b.block = 0;
  }
  m_buffer_next = 0;

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

  // Page buffer check: is this page already in one of the buffers?
  bool hit = false;
  if (m_buffer_enabled) {
    for (auto &b : m_buffers) {
      if (b.valid && b.page == page && b.block == block) {
        hit = true;
        break;
      }
    }
  }
  if (hit) {
    // Buffer hit — data already in a page register, minimal latency.
    // (OCP §5.3.1.7: cache-hit reads are served immediately if there are
    // no ordering violations; we model single-bank strict ordering by the
    // sub-array state machine itself.)
    m_timing_remaining = m_tBUFF;
    n_buffer_hits++;
  } else {
    // Buffer miss — full NAND cell read required
    m_timing_remaining = m_tR;
    n_buffer_misses++;
    // The read loads this page into a buffer slot (round-robin replacement).
    if (m_buffer_enabled) {
      hbf_page_buffer_t &slot = m_buffers[m_buffer_next];
      slot.valid  = true;
      slot.page   = page;
      slot.block  = block;
      m_buffer_next = (m_buffer_next + 1) % m_buffers.size();
    }
  }

  n_reads++;
}

void hbf_subarray_t::start_program(unsigned page, unsigned block) {
  m_state      = PROGRAMMING;
  m_curr_page  = page;
  m_curr_block = block;

  m_timing_remaining = m_tPROG;

  // Program loads data into a page register before writing to NAND cells,
  // so a buffer slot now holds this page.
  if (m_buffer_enabled) {
    hbf_page_buffer_t &slot = m_buffers[m_buffer_next];
    slot.valid  = true;
    slot.page   = page;
    slot.block  = block;
    m_buffer_next = (m_buffer_next + 1) % m_buffers.size();
  }

  n_writes++;
  pe_cycles++;
}

void hbf_subarray_t::start_erase(unsigned block) {
  m_state      = ERASING;
  m_curr_block = block;

  m_timing_remaining = m_tBERS;

  // Erasing a block invalidates all buffer slots holding its pages.
  for (auto &b : m_buffers) {
    if (b.valid && b.block == block) {
      b.valid = false;
    }
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
