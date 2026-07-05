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
// 1. Convert 64B address → page address
// 2. Check MSHR: if page already pending, coalesce (MSHR hit)
// 3. Otherwise, create new MSHR entry and queue for scheduling
// ============================================================================
void hbf_controller_t::push(mem_fetch *data) {
  unsigned long long current_cycle =
      m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle;

  n_total_requests++;

  unsigned long long page_addr = addr_to_page(data->get_addr());

  // Check MSHR (only if enabled): is this page already being fetched?
  if (m_config->hbf_mshr_enabled) {
    auto it = m_mshr.find(page_addr);
    if (it != m_mshr.end()) {
      // MSHR HIT: coalesce — add this request to the pending list
      it->second.pending.push_back(data);
      n_mshr_hits++;
      return;
    }
  }

  // MSHR MISS (or MSHR disabled): create new entry for this request
  mshr_entry_t entry;
  entry.phys_page   = page_addr;
  entry.in_flight   = false;
  entry.issue_cycle = current_cycle;
  entry.pending.push_back(data);

  // FTL translation (if enabled)
  if (m_ftl) {
    hbf_phys_addr_t phys = m_ftl->translate(page_addr, data->get_is_write());
    entry.subarray_id = phys.subarray;
    entry.block_id    = phys.block;
    entry.page_offset = phys.page;
  } else {
    // Direct-mapped fallback: distribute pages round-robin across sub-arrays
    entry.subarray_id = page_addr % m_num_subarrays;
    entry.block_id    = (page_addr / m_num_subarrays) %
                        (1024 * 1024 / m_config->hbf_page_size);  // rough
    entry.page_offset = 0;
  }

  m_mshr[page_addr] = entry;
  m_mshr_queue.push_back(page_addr);

  // Track queue depth
  if (m_mshr_queue.size() > max_queue_depth) {
    max_queue_depth = m_mshr_queue.size();
  }
}

// ============================================================================
// cycle() — per-cycle HBF controller logic
//
// 1. Sub-array state machines: decrement timing counters
// 2. Check for completed operations → return data for all pending requests
// 3. Schedule new operations: assign MSHR entries to idle sub-arrays
// ============================================================================
void hbf_controller_t::cycle() {
  unsigned long long current_cycle =
      m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle;

  // Track active cycles
  if (!m_mshr_queue.empty() || m_mshr.size() > 0) {
    n_cycles_active++;
  }

  // Step 1: Cycle all sub-arrays (decrement timing counters)
  for (unsigned i = 0; i < m_num_subarrays; i++) {
    hbf_subarray_t *sa = m_subarrays[i];

    // Check if this sub-array just completed an operation
    hbf_subarray_t::state_t prev_state = sa->get_state();
    sa->cycle();
    hbf_subarray_t::state_t new_state = sa->get_state();

    // If transitioned from non-IDLE to IDLE, operation completed
    if (prev_state != hbf_subarray_t::IDLE && new_state == hbf_subarray_t::IDLE) {
      // Find the MSHR entry that was using this sub-array
      unsigned completed_page = sa->get_curr_page();
      // Walk MSHR to find the matching entry
      for (auto &kv : m_mshr) {
        if (kv.second.in_flight && kv.second.subarray_id == i) {
          // Return data for all pending requests on this page
          for (mem_fetch *mf : kv.second.pending) {
            if (mf->get_access_type() != L1_WRBK_ACC &&
                mf->get_access_type() != L2_WRBK_ACC) {
              mf->set_reply();
            }
            if (!m_returnq->full()) {
              m_returnq->push(mf);
            }
            // Track statistics
            unsigned long long latency = current_cycle - kv.second.issue_cycle;
            if (mf->get_is_write()) {
              total_write_latency += latency;
              n_bytes_written += mf->get_data_size();
            } else {
              total_read_latency += latency;
              n_bytes_read += mf->get_data_size();
            }
          }
          // Remove MSHR entry
          m_mshr.erase(kv.first);
          break;
        }
      }
    }
  }

  // Step 2: Schedule new operations
  schedule_operations();
}

// ============================================================================
// schedule_operations() — assign MSHR queue entries to idle sub-arrays
//
// Simple round-robin: walk the MSHR queue, assign each entry to the first
// available idle sub-array. Limited by max_active (power cap).
// ============================================================================
void hbf_controller_t::schedule_operations() {
  unsigned active_count = 0;
  for (unsigned i = 0; i < m_num_subarrays; i++) {
    if (!m_subarrays[i]->is_idle()) active_count++;
  }

  // Don't exceed power-limited maximum
  if (active_count >= m_max_active) return;

  auto it = m_mshr_queue.begin();
  while (it != m_mshr_queue.end() && active_count < m_max_active) {
    unsigned long long page_addr = *it;
    auto mshr_it = m_mshr.find(page_addr);

    // Entry might have been removed (edge case)
    if (mshr_it == m_mshr.end()) {
      it = m_mshr_queue.erase(it);
      continue;
    }

    // Already in flight? Skip
    if (mshr_it->second.in_flight) {
      ++it;
      continue;
    }

    // Find an idle sub-array
    unsigned target_sa = mshr_it->second.subarray_id;
    bool assigned = false;

    // Try the preferred sub-array first
    if (m_subarrays[target_sa]->is_idle()) {
      assigned = true;
    } else {
      // Fallback: find any idle sub-array
      for (unsigned i = 0; i < m_num_subarrays; i++) {
        if (m_subarrays[i]->is_idle()) {
          target_sa = i;
          mshr_it->second.subarray_id = i;
          assigned = true;
          break;
        }
      }
    }

    if (assigned) {
      hbf_subarray_t *sa = m_subarrays[target_sa];
      mem_fetch *first_mf = mshr_it->second.pending.front();

      if (first_mf->get_is_write()) {
        // Write: need to program a page
        // If FTL says block needs erase, do that first (simplified: always program)
        sa->start_program(mshr_it->second.page_offset,
                          mshr_it->second.block_id);
        n_page_programs++;
      } else {
        // Read
        sa->start_read(mshr_it->second.page_offset,
                       mshr_it->second.block_id);
        n_page_reads++;
      }

      mshr_it->second.in_flight = true;
      mshr_it->second.subarray_id = target_sa;
      it = m_mshr_queue.erase(it);
      active_count++;
    } else {
      ++it;  // No idle sub-array available, try next cycle
    }
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
