// HBF Phase 2: Cycle-Accurate HBF Controller Implementation
//
// Models the HBF logic die's behavior:
//   1. Receive 64B cache-line requests from L2 misses
//   2. MSHR coalescing: group requests by 4KB NAND page
//   3. Schedule: assign pages to idle sub-arrays (up to max_active)
//   4. Sub-array state machines: IDLE → READING/PROGRAMMING/ERASING → IDLE
//   5. Complete: when sub-array finishes, return data for all pending requests

#include "hbf_controller.h"
#include <stdio.h>
#include "../abstract_hardware_model.h"
#include "gpu-sim.h"
#include "l2cache.h"
#include "mem_fetch.h"
#include "mem_latency_stat.h"
#include "hbf_ftl.h"

// ============================================================================
// Constructor — allocate sub-arrays and queues
// ============================================================================
hbf_controller_t::hbf_controller_t(unsigned partition_id,
                                   const memory_config *config,
                                   memory_stats_t *stats,
                                   memory_partition_unit *mp, gpgpu_sim *gpu)
    : m_id(partition_id),
      m_config(config),
      m_stats(stats),
      m_memory_partition_unit(mp),
      m_gpu(gpu) {
  m_num_subarrays = config->hbf_num_subarrays;
  m_max_active    = config->hbf_max_active;

  // Allocate sub-array array
  m_subarrays = new hbf_subarray_t *[m_num_subarrays];
  for (unsigned i = 0; i < m_num_subarrays; i++) {
    m_subarrays[i] = new hbf_subarray_t(i, m_config);
  }

  // Return queue: delay=0, size from DRAM return queue config
  m_returnq = new fifo_pipeline<mem_fetch>(
      "hbf_returnq", 0, config->gpgpu_dram_return_queue_size);

  // FTL if enabled
  if (m_config->hbf_ftl_enabled) {
    m_ftl = new hbf_ftl_t(m_config);
  } else {
    m_ftl = NULL;
  }

  // Write buffer: max entries before forced flush (default 32)
  m_write_buffer_max = 32;

  // Statistics
  n_page_reads       = 0;
  n_page_programs    = 0;
  n_block_erases     = 0;
  n_mshr_hits        = 0;
  n_total_requests   = 0;
  total_read_latency = 0;
  total_write_latency = 0;
  max_queue_depth    = 0;
  n_cycles_active    = 0;
  n_bytes_read       = 0;
  n_bytes_written    = 0;
}

hbf_controller_t::~hbf_controller_t() {
  for (unsigned i = 0; i < m_num_subarrays; i++) {
    delete m_subarrays[i];
  }
  delete[] m_subarrays;
  delete m_returnq;
  if (m_ftl) delete m_ftl;
}

// ============================================================================
// Address to page translation
// ============================================================================
unsigned long long hbf_controller_t::addr_to_page(new_addr_type addr) const {
  return addr / m_config->hbf_page_size;
}

// ============================================================================
// Backpressure check
// ============================================================================
bool hbf_controller_t::full(bool is_write) const {
  unsigned max_q = m_config->hbf_max_outstanding;
  if (max_q == 0) return false;  // unlimited
  return m_mshr.size() >= max_q;
}

// ============================================================================
// push() — entry point for L2 miss requests
//
// Reads flow directly into MSHR (same as before).
// Writes go through a write buffer first: multiple writes to the same page
// are coalesced in the buffer, then flushed to MSHR when the buffer is full
// or when a read to the same page arrives.
// ============================================================================
void hbf_controller_t::push(mem_fetch *data) {
  unsigned long long current_cycle =
      m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle;

  n_total_requests++;

  unsigned long long page_addr = addr_to_page(data->get_addr());

  // Writes: buffer first, then flush to MSHR
  if (data->get_is_write()) {
    auto wb_it = m_write_buffer.find(page_addr);
    if (wb_it != m_write_buffer.end()) {
      // Coalesce into existing write buffer entry
      wb_it->second.requests.push_back(data);
      return;
    }
    // New write buffer entry
    write_buffer_entry_t wb;
    wb.page_addr = page_addr;
    wb.first_arrival = current_cycle;
    wb.requests.push_back(data);
    m_write_buffer[page_addr] = wb;

    // Flush if buffer is full
    if (m_write_buffer.size() >= m_write_buffer_max) {
      flush_write_buffer();
    }
    return;
  }

  // Reads: flush any buffered writes to same page first (read-after-write)
  auto wb_it = m_write_buffer.find(page_addr);
  if (wb_it != m_write_buffer.end()) {
    flush_write_buffer_entry(wb_it->first);
  }

  // Check MSHR (only if enabled)
  if (m_config->hbf_mshr_enabled) {
    auto it = m_mshr.find(page_addr);
    if (it != m_mshr.end()) {
      it->second.pending.push_back(data);
      n_mshr_hits++;
      return;
    }
  }

  // Create MSHR entry for read
  unsigned long long entry_key = m_config->hbf_mshr_enabled
                                     ? page_addr
                                     : (page_addr << 20) | n_total_requests;
  mshr_entry_t entry;
  entry.phys_page   = page_addr;
  entry.op_state    = OP_WAITING;
  entry.needs_erase = false;
  entry.pending.push_back(data);

  if (m_ftl) {
    hbf_phys_addr_t phys = m_ftl->translate(page_addr, false);
    entry.subarray_id = phys.subarray;
    entry.block_id    = phys.block;
    entry.page_offset = phys.page;
  } else {
    entry.subarray_id = page_addr % m_num_subarrays;
    entry.block_id    = (page_addr / m_num_subarrays) % 1024;
    entry.page_offset = 0;
  }

  m_mshr[entry_key] = entry;
  m_mshr_queue.push_back(entry_key);
  if (m_mshr_queue.size() > max_queue_depth) max_queue_depth = m_mshr_queue.size();
}

// Flush all buffered writes to MSHR
void hbf_controller_t::flush_write_buffer() {
  std::vector<unsigned long long> keys;
  for (auto &kv : m_write_buffer) keys.push_back(kv.first);
  for (auto key : keys) flush_write_buffer_entry(key);
  m_write_buffer.clear();
}

// Flush a single write buffer entry to MSHR
void hbf_controller_t::flush_write_buffer_entry(unsigned long long page_addr) {
  auto it = m_write_buffer.find(page_addr);
  if (it == m_write_buffer.end()) return;
  write_buffer_entry_t &wb = it->second;
  if (wb.requests.empty()) { m_write_buffer.erase(it); return; }

  unsigned long long wb_page_addr = wb.page_addr;
  unsigned long long entry_key = m_config->hbf_mshr_enabled
                                     ? wb_page_addr
                                     : (wb_page_addr << 20) | n_total_requests;

  mshr_entry_t entry;
  entry.phys_page   = page_addr;
  entry.op_state    = OP_WAITING;
  entry.needs_erase = false;
  entry.pending     = std::move(wb.requests);

  if (m_ftl) {
    hbf_phys_addr_t phys = m_ftl->translate(page_addr, true);
    entry.subarray_id = phys.subarray;
    entry.block_id    = phys.block;
    entry.page_offset = phys.page;

    // Check if target block is erased — NAND can't program unless erased
    if (!m_ftl->is_block_erased(phys.subarray, phys.block)) {
      entry.needs_erase = true;
    }
  } else {
    entry.subarray_id = page_addr % m_num_subarrays;
    entry.block_id    = (page_addr / m_num_subarrays) % 1024;
    entry.page_offset = 0;
  }

  m_mshr[entry_key] = entry;
  m_mshr_queue.push_back(entry_key);
  if (m_mshr_queue.size() > max_queue_depth) max_queue_depth = m_mshr_queue.size();
}

// ============================================================================
// cycle() — per-cycle HBF controller logic
//
// 1. Sub-array state machines: decrement timing counters
// 2. Completed operations: READ → return data; ERASE → schedule program
// 3. Schedule new operations: assign MSHR entries to idle sub-arrays
// 4. Flush write buffer if entries are old enough
// ============================================================================
void hbf_controller_t::cycle() {
  unsigned long long current_cycle =
      m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle;

  if (!m_mshr_queue.empty() || m_mshr.size() > 0 || !m_write_buffer.empty()) {
    n_cycles_active++;
  }

  // Step 1: Cycle all sub-arrays, track completions
  struct completion_t {
    unsigned long long mshr_key;
    unsigned subarray_id;
    hbf_subarray_t::state_t prev_state;
  };
  std::vector<completion_t> completions;

  for (unsigned i = 0; i < m_num_subarrays; i++) {
    hbf_subarray_t *sa = m_subarrays[i];
    hbf_subarray_t::state_t prev = sa->get_state();
    sa->cycle();
    if (prev != hbf_subarray_t::IDLE && sa->get_state() == hbf_subarray_t::IDLE) {
      for (auto &kv : m_mshr) {
        if (kv.second.op_state != OP_WAITING && kv.second.subarray_id == i) {
          completions.push_back({kv.first, i, prev});
          break;
        }
      }
    }
  }

  // Step 2: Process completions
  for (auto &c : completions) {
    auto it = m_mshr.find(c.mshr_key);
    if (it == m_mshr.end()) continue;

    if (c.prev_state == hbf_subarray_t::ERASING) {
      // Erase just completed → block is now erased, mark and schedule program
      if (m_ftl)
        m_ftl->mark_block_erased(it->second.subarray_id, it->second.block_id);
      it->second.needs_erase = false;
      it->second.op_state = OP_WAITING;
      // Don't erase from queue — it will be picked up for programming next cycle
    } else {
      // Read or program completed → return data to requesters
      unsigned long long op_cycle = 0;
      for (mem_fetch *mf : it->second.pending) {
        if (mf->get_access_type() != L1_WRBK_ACC &&
            mf->get_access_type() != L2_WRBK_ACC) {
          mf->set_reply();
        }
        if (!m_returnq->full()) m_returnq->push(mf);
        unsigned long long latency = current_cycle - it->second.issue_cycle;
        if (mf->get_is_write()) {
          total_write_latency += latency; n_bytes_written += mf->get_data_size();
        } else {
          total_read_latency += latency; n_bytes_read += mf->get_data_size();
        }
      }
      m_mshr.erase(it);
    }
  }

  // Step 3: Flush write buffer entries that have been waiting too long
  static const unsigned WRITE_BUFFER_TIMEOUT = 1000;
  auto wb_it = m_write_buffer.begin();
  while (wb_it != m_write_buffer.end()) {
    if (current_cycle - wb_it->second.first_arrival > WRITE_BUFFER_TIMEOUT) {
      flush_write_buffer_entry((wb_it++)->first);
    } else {
      ++wb_it;
    }
  }

  // Step 4: Schedule new operations
  schedule_operations();
}

// ============================================================================
// schedule_operations() — assign MSHR queue entries to idle sub-arrays
//
// Read entries: assign to sub-array, issue READ.
// Write entries:
//   - If block not erased: issue ERASE first → later issue PROGRAM.
//   - If block erased: issue PROGRAM directly.
// Power-limited by max_active (count of non-idle sub-arrays).
// ============================================================================
void hbf_controller_t::schedule_operations() {
  unsigned active_count = 0;
  for (unsigned i = 0; i < m_num_subarrays; i++) {
    if (!m_subarrays[i]->is_idle()) active_count++;
  }
  if (active_count >= m_max_active) return;

  auto it = m_mshr_queue.begin();
  while (it != m_mshr_queue.end() && active_count < m_max_active) {
    auto mshr_it = m_mshr.find(*it);
    if (mshr_it == m_mshr.end()) { it = m_mshr_queue.erase(it); continue; }
    if (mshr_it->second.op_state != OP_WAITING) { ++it; continue; }

    // Find idle sub-array (prefer preferred, fallback to any)
    unsigned sa_id = mshr_it->second.subarray_id;
    if (!m_subarrays[sa_id]->is_idle()) {
      bool found = false;
      for (unsigned i = 0; i < m_num_subarrays; i++) {
        if (m_subarrays[i]->is_idle()) { sa_id = i; mshr_it->second.subarray_id = i; found = true; break; }
      }
      if (!found) { ++it; continue; }
    }

    hbf_subarray_t *sa = m_subarrays[sa_id];
    mem_fetch *first_mf = mshr_it->second.pending.front();

    if (first_mf->get_is_write()) {
      // ═══ Write path: erase-before-write ═══
      if (mshr_it->second.needs_erase) {
        // Block not erased → issue ERASE first
        if (m_ftl) m_ftl->mark_block_erasing(sa_id, mshr_it->second.block_id);
        sa->start_erase(mshr_it->second.block_id);
        mshr_it->second.op_state = OP_ERASING;
        mshr_it->second.issue_cycle = m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle;
        n_block_erases++;
      } else {
        // Block erased → issue PROGRAM
        sa->start_program(mshr_it->second.page_offset,
                          mshr_it->second.block_id);
        mshr_it->second.op_state = OP_PROGRAMMING;
        mshr_it->second.issue_cycle = m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle;
        n_page_programs++;
      }
    } else {
      // Read
      sa->start_read(mshr_it->second.page_offset, mshr_it->second.block_id);
      mshr_it->second.op_state = OP_READING;
      mshr_it->second.issue_cycle = m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle;
      n_page_reads++;
    }

    it = m_mshr_queue.erase(it);
    active_count++;
  }
}

// ============================================================================
// Return queue interface (standard dram_t pattern)
// ============================================================================
mem_fetch *hbf_controller_t::return_queue_pop() {
  return m_returnq->pop();
}

mem_fetch *hbf_controller_t::return_queue_top() {
  return m_returnq->top();
}

unsigned hbf_controller_t::que_length() const {
  return m_mshr_queue.size() + m_mshr.size();
}

// ============================================================================
// Statistics printing
// ============================================================================
void hbf_controller_t::print_stat(FILE *simFile) {
  fprintf(simFile, "\n========= HBF Controller [%d] Statistics (Phase 2) =========\n",
          m_id);
  fprintf(simFile, "HBF Sub-arrays:         %u\n", m_num_subarrays);
  fprintf(simFile, "HBF Page Reads:         %llu\n", n_page_reads);
  fprintf(simFile, "HBF Page Programs:      %llu\n", n_page_programs);
  fprintf(simFile, "HBF Block Erases:       %llu\n", n_block_erases);
  fprintf(simFile, "HBF Total Requests:     %llu\n", n_total_requests);
  fprintf(simFile, "HBF MSHR Hits:          %llu (%.1f%% coalesced)\n",
          n_mshr_hits,
          n_total_requests > 0
              ? 100.0 * n_mshr_hits / n_total_requests
              : 0.0);
  fprintf(simFile, "HBF Bytes Read:         %llu\n", n_bytes_read);
  fprintf(simFile, "HBF Bytes Written:      %llu\n", n_bytes_written);
  if (n_page_reads + n_page_programs > 0) {
    unsigned long long total_lat = total_read_latency + total_write_latency;
    unsigned long long total_ops = n_page_reads + n_page_programs;
    fprintf(simFile, "HBF Avg Op Latency:     %llu cycles\n",
            total_lat / total_ops);
  }
  fprintf(simFile, "HBF Max Queue Depth:    %u\n", max_queue_depth);
  fprintf(simFile, "HBF Cycles Active:      %llu\n", n_cycles_active);

  if (m_ftl) {
    m_ftl->print_stat(simFile);
  }

  // Per-subarray summary
  unsigned idle_count = 0;
  for (unsigned i = 0; i < m_num_subarrays; i++) {
    if (m_subarrays[i]->is_idle()) idle_count++;
  }
  fprintf(simFile, "HBF Active Sub-arrays:  %u / %u\n",
          m_num_subarrays - idle_count, m_num_subarrays);

  fprintf(simFile, "==========================================================\n\n");
}
