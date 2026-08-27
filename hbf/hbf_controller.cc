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

  // All HBF timing counters count DRAM-clock ticks (hbf_cycle runs in the
  // DRAM clock domain, exactly like dram_t). Record the clock frequencies
  // in MHz for statistics reporting (ticks -> us) and documentation.
  // NOTE: gpgpu_sim_config stores frequencies in Hz (the MhZ macro
  // multiplies by 1e6), so divide by 1e6 here.
  m_dram_freq_mhz = gpu->get_config().get_dram_freq() / 1e6;
  m_core_freq_mhz = gpu->get_config().get_core_freq() / 1e6;

  // ── Host channels (v0.4) ────────────────────────────────────────────
  // Each channel owns a contiguous slice of the sub-array array. The
  // configured channel count is clamped to [1, 16] (OCP §4.5) and to the
  // number of sub-arrays (every channel must own at least one).
  m_num_channels = config->hbf_num_channels;
  if (m_num_channels == 0) m_num_channels = 1;
  if (m_num_channels > 16) m_num_channels = 16;
  if (m_num_channels > m_num_subarrays) m_num_channels = m_num_subarrays;
  m_channel_map_mode = config->hbf_channel_map;
  m_pages_per_channel = (config->hbf_size / config->hbf_page_size) /
                        m_num_channels;
  // Per-channel interface bandwidth: GB/s -> bytes per DRAM tick.
  double bw_bytes_per_tick =
      config->hbf_channel_bw_gbps * 1000.0 / m_dram_freq_mhz;
  m_channels = new hbf_channel_t *[m_num_channels];
  unsigned per_ch_subarrays = m_num_subarrays / m_num_channels;
  for (unsigned c = 0; c < m_num_channels; c++) {
    unsigned first = c * per_ch_subarrays;
    unsigned count =
        (c == m_num_channels - 1) ? (m_num_subarrays - first) : per_ch_subarrays;
    m_channels[c] = new hbf_channel_t(c, first, count, bw_bytes_per_tick);
  }

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

  // Write buffer: max entries before forced flush (configurable, v0.4)
  m_write_buffer_max =
      config->hbf_write_buffer_entries > 0 ? config->hbf_write_buffer_entries : 32;

  // Scheduling policy (v0.4): 0=FCFS, 1=read-priority, 2=write-drain
  m_scheduler = config->hbf_scheduler;
  if (m_scheduler < 0 || m_scheduler > 2) m_scheduler = 0;
  m_write_drain_high = config->hbf_write_drain_high;
  m_write_drain_until = 0;
  n_write_drain_windows = 0;
  n_read_deferred_in_window = 0;

  // v0.4: Shared page cache (0 entries = disabled)
  m_page_cache = new hbf_page_cache_t(m_config->hbf_cache_entries);

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
  n_gc_stall_cycles  = 0;
  n_cache_hits       = 0;
  n_cache_misses     = 0;
  n_partial_page_programs = 0;
  n_write_timeout_flushes = 0;
  n_idle_drain_flushes    = 0;
  n_read_requests = 0;
  max_read_latency = 0;
}

hbf_controller_t::~hbf_controller_t() {
  for (unsigned i = 0; i < m_num_subarrays; i++) {
    delete m_subarrays[i];
  }
  delete[] m_subarrays;
  for (unsigned c = 0; c < m_num_channels; c++) {
    delete m_channels[c];
  }
  delete[] m_channels;
  delete m_returnq;
  if (m_ftl) delete m_ftl;
  delete m_page_cache;
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
  unsigned ch = channel_of_page(page_addr);
  m_channels[ch]->n_requests++;

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
    flush_write_buffer_entry(wb_it->first, true);
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
  entry.channel_id  = ch;
  entry.op_state    = OP_WAITING;
  entry.needs_erase = false;
  entry.delivery_pending = false;
  entry.fill_cache_on_deliver = false;
  entry.cache_hit_ready_cycle = 0;
  entry.pending.push_back(data);

  if (m_ftl) {
    hbf_phys_addr_t phys = m_ftl->translate(page_addr, false);
    entry.subarray_id = phys.subarray;
    entry.block_id    = phys.block;
    entry.page_offset = phys.page;
  } else {
    // Non-FTL mode: bind to a sub-array of the page's own channel.
    entry.subarray_id = m_channels[ch]->subarray_of_local(page_addr);
    entry.block_id    = 0;
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
  for (auto key : keys) flush_write_buffer_entry(key, false);
  m_write_buffer.clear();
}

// Flush a single write buffer entry to MSHR.
// `force` bypasses the per-channel outstanding-write limit — used by the
// read-after-write path, where the write MUST reach the MSHR so the read
// coalesces onto it (ordering). Regular flushes (buffer full / timeout)
// defer when the channel is saturated (OCP §5.4.1.7 product limit).
void hbf_controller_t::flush_write_buffer_entry(unsigned long long page_addr,
                                                bool force) {
  auto it = m_write_buffer.find(page_addr);
  if (it == m_write_buffer.end()) return;
  write_buffer_entry_t &wb = it->second;
  if (wb.requests.empty()) { m_write_buffer.erase(it); return; }

  unsigned ch = channel_of_page(wb.page_addr);
  if (!force &&
      !m_channels[ch]->can_accept_write(m_config->hbf_max_outstanding_writes)) {
    m_channels[ch]->n_writes_deferred++;
    return;  // channel saturated — keep buffering
  }

  // Per-page serialization: if an MSHR entry for this page is already in
  // flight (a read or an earlier write), do NOT create a second entry —
  // the MSHR is keyed by page address and a second entry would overwrite
  // the first, silently losing its pending requests. Keep the data in the
  // write buffer and retry on a later cycle (the timeout/idle loops retry
  // every cycle until the in-flight entry completes).
  if (m_mshr.find(wb.page_addr) != m_mshr.end()) {
    m_channels[ch]->n_writes_deferred++;
    return;
  }

  unsigned long long wb_page_addr = wb.page_addr;
  unsigned long long entry_key = m_config->hbf_mshr_enabled
                                     ? wb_page_addr
                                     : (wb_page_addr << 20) | n_total_requests;

  mshr_entry_t entry;
  entry.phys_page   = page_addr;
  entry.channel_id  = ch;
  entry.op_state    = OP_WAITING;
  entry.needs_erase = false;
  entry.delivery_pending = false;
  entry.fill_cache_on_deliver = false;
  entry.cache_hit_ready_cycle = 0;
  entry.pending     = std::move(wb.requests);
  // Count partial-page programs: OCP §5.4.1 requires a FULL 4KiB page to be
  // accumulated before programming (the Base die controller aggregates
  // complete 4KiB). A flush with less data is a partial-page program — a
  // write-amplification source that the paper quantifies.
  unsigned long long pending_bytes = 0;
  for (mem_fetch *mf : entry.pending) pending_bytes += mf->get_data_size();
  if (pending_bytes < m_config->hbf_page_size) {
    n_partial_page_programs++;
  }
  m_channels[ch]->n_outstanding_writes++;

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
    entry.subarray_id = m_channels[ch]->subarray_of_local(page_addr);
    entry.block_id    = 0;
    entry.page_offset = 0;
  }

  m_mshr[entry_key] = entry;
  m_mshr_queue.push_back(entry_key);
  if (m_mshr_queue.size() > max_queue_depth) max_queue_depth = m_mshr_queue.size();
  // Remove the drained entry from the write buffer so later writes to the
  // same page create a FRESH buffer entry (a stale empty entry would be
  // silently reused, batching new writes into a page whose MSHR entry is
  // already in flight — the root cause of the double-flush/lost-request
  // bug in v0.4.0).
  m_write_buffer.erase(it);
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

  // Replenish per-channel interface transfer credits (UCIe/AXI link BW,
  // OCP §4.2 Table 2: per-channel raw BW x AXI link efficiency).
  for (unsigned c = 0; c < m_num_channels; c++) {
    m_channels[c]->replenish_credit();
  }

  // Step 1: Cycle only the ACTIVE sub-arrays, collect completions.
  // (Scanning all 16K sub-arrays per cycle was a ~10x simulation slowdown;
  // an op finishes only from an active sub-array, so nothing is missed.)
  for (auto it = m_active_subarrays.begin(); it != m_active_subarrays.end();) {
    unsigned i = *it;
    hbf_subarray_t *sa = m_subarrays[i];
    sa->cycle();
    if (sa->get_state() == hbf_subarray_t::IDLE) {
      // Find the MSHR entry this sub-array was serving.
      for (auto &kv : m_mshr) {
        if (kv.second.op_state != OP_WAITING && kv.second.subarray_id == i &&
            !kv.second.delivery_pending) {
          if (kv.second.op_state == OP_ERASING) {
            // Erase completion: block is now erased; no data to deliver.
            // Re-queue the entry so schedule_operations() issues the PROGRAM.
            // (Without this the entry sits in m_mshr but never gets picked
            // up again — the write-path deadlock.)
            if (m_ftl)
              m_ftl->mark_block_erased(kv.second.subarray_id,
                                       kv.second.block_id);
            kv.second.needs_erase = false;
            kv.second.op_state = OP_WAITING;
            m_mshr_queue.push_back(kv.first);
          } else {
            // Read/program completion: mark for delivery (retried each cycle
            // until the return queue has room for ALL pending replies).
            kv.second.fill_cache_on_deliver =
                (kv.second.op_state == OP_READING);
            kv.second.delivery_pending = true;
          }
          break;
        }
      }
      it = m_active_subarrays.erase(it);
    } else {
      ++it;
    }
  }

  // Step 2: Deliver completed read/program data to requesters.
  // Deferred until the return queue can hold every pending reply — dropping
  // one would permanently lose the reply and deadlock the core — AND the
  // owning channel's link has transfer credit (interface bandwidth model).
  for (auto it = m_mshr.begin(); it != m_mshr.end();) {
    if (!it->second.delivery_pending) { ++it; continue; }
    if (m_returnq->get_length() + it->second.pending.size() >
        m_returnq->get_max_len()) { ++it; continue; }  // retry next cycle
    // Per-channel interface bandwidth: read data flows back over the
    // channel's UCIe/AXI link; write completions are control-only.
    double ret_bytes = 0.0;
    bool is_write_entry = false;
    for (mem_fetch *mf : it->second.pending) {
      if (mf->get_is_write()) { is_write_entry = true; }
      else { ret_bytes += mf->get_data_size(); }
    }
    hbf_channel_t *ch = m_channels[it->second.channel_id];
    if (ret_bytes > 0.0 && !ch->try_consume_credit(ret_bytes)) {
      ch->n_transfer_stall_ticks++;
      ++it; continue;  // link saturated — retry next tick
    }
    // v0.4: Fill page cache on read completion
    if (it->second.fill_cache_on_deliver && m_page_cache->enabled()) {
      m_page_cache->insert(it->second.phys_page);
    }
    for (mem_fetch *mf : it->second.pending) {
      if (mf->get_access_type() != L1_WRBK_ACC &&
          mf->get_access_type() != L2_WRBK_ACC) {
        mf->set_reply();
      }
      m_returnq->push(mf);
      unsigned long long latency = current_cycle - it->second.issue_cycle;
      if (mf->get_is_write()) {
        total_write_latency += latency; n_bytes_written += mf->get_data_size();
      } else {
        total_read_latency += latency; n_bytes_read += mf->get_data_size();
        n_read_requests++;
        if (latency > max_read_latency) max_read_latency = latency;
      }
    }
    if (is_write_entry && ch->n_outstanding_writes > 0) {
      ch->n_outstanding_writes--;  // write completed on the channel
    }
    it = m_mshr.erase(it);
  }

  // Step 2b: Deliver page-cache hit completions (no sub-array involved).
  // Same retry-until-room + channel-credit policy.
  for (auto it = m_mshr.begin(); it != m_mshr.end();) {
    if (it->second.cache_hit_ready_cycle == 0 ||
        current_cycle < it->second.cache_hit_ready_cycle) { ++it; continue; }
    if (m_returnq->get_length() + it->second.pending.size() >
        m_returnq->get_max_len()) { ++it; continue; }  // retry next cycle
    double ret_bytes = 0.0;
    for (mem_fetch *mf : it->second.pending) {
      if (!mf->get_is_write()) ret_bytes += mf->get_data_size();
    }
    hbf_channel_t *ch = m_channels[it->second.channel_id];
    if (ret_bytes > 0.0 && !ch->try_consume_credit(ret_bytes)) {
      ch->n_transfer_stall_ticks++;
      ++it; continue;  // link saturated — retry next tick
    }
    for (mem_fetch *mf : it->second.pending) {
      if (mf->get_access_type() != L1_WRBK_ACC &&
          mf->get_access_type() != L2_WRBK_ACC) {
        mf->set_reply();
      }
      m_returnq->push(mf);
      unsigned long long latency = current_cycle - it->second.issue_cycle;
      total_read_latency += latency;
      n_bytes_read += mf->get_data_size();
      n_read_requests++;
      if (latency > max_read_latency) max_read_latency = latency;
    }
    it = m_mshr.erase(it);
  }

  // Step 3a: FTL housekeeping (GC state machine)
  if (m_ftl) {
    m_ftl->cycle();
  }

  // Step 3a2: write-drain window lifecycle (v0.4, scheduler mode 2).
  // A window opens when PENDING writes (buffered entries plus queued but
  // unscheduled MSHR write entries -- timeout flushes move writes out of
  // the buffer, so buffer occupancy alone underestimates pressure) reach
  // the high-water mark, and lasts at most hbf_write_drain_maxwait ticks
  // (the read-starvation bound), closing early when writes drain.
  if (m_scheduler == 2) {
    unsigned pending_writes = (unsigned)m_write_buffer.size();
    for (auto &k : m_mshr_queue) {
      auto it = m_mshr.find(k);
      if (it != m_mshr.end() && it->second.op_state == OP_WAITING &&
          !it->second.pending.empty() &&
          it->second.pending.front()->get_is_write()) {
        pending_writes++;
      }
    }

    if (m_write_drain_until == 0 &&
        pending_writes >= m_write_drain_high) {

      m_write_drain_until = current_cycle + m_config->hbf_write_drain_maxwait;
      n_write_drain_windows++;
    } else if (m_write_drain_until > 0 &&
               (current_cycle > m_write_drain_until ||
                pending_writes < m_write_drain_high)) {
      m_write_drain_until = 0;
    }
  }

  // Step 3b: Flush write buffer entries that have been waiting too long.
  // The deadline is host-configured (OCP §5.4.1.6: the host sets the time
  // limit within which a full 4KiB must be assembled; HBF errors out on
  // violation — here we flush a partial page and count it).
  unsigned timeout = m_config->hbf_write_agg_timeout;
  auto wb_it = m_write_buffer.begin();
  while (wb_it != m_write_buffer.end()) {
    if (current_cycle - wb_it->second.first_arrival > timeout) {
      n_write_timeout_flushes++;
      flush_write_buffer_entry((wb_it++)->first, false);
    } else {
      ++wb_it;
    }
  }

  // Step 4: Schedule new operations
  schedule_operations();

  // Step 5: Idle drain (v0.4) — when the device has nothing in flight and
  // the oldest buffered write has been waiting for a quiet period, flush it
  // so tail writes are not silently dropped at kernel end. The quiet period
  // (100 ticks) preserves 4KiB write aggregation: a flush right after the
  // FIRST store of a page would split the page into a partial program and
  // re-open the double-flush hazard (v0.4.0 bug).
  if (!m_write_buffer.empty() && m_mshr.empty() && m_mshr_queue.empty()) {
    unsigned long long oldest = ~0ull;
    for (auto &kv : m_write_buffer)
      if (kv.second.first_arrival < oldest) oldest = kv.second.first_arrival;
    if (current_cycle - oldest > 100) {
      n_idle_drain_flushes++;
      flush_write_buffer();
    }
  }
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
  // active-count maintained incrementally by m_active_subarrays — scanning
  // all sub-arrays here every cycle was the main simulation bottleneck.
  unsigned active_count = (unsigned)m_active_subarrays.size();
  if (active_count >= m_max_active) return;

  // Scheduling policy (v0.4): pick the queue entry to issue next.
  //   mode 0 (FCFS): first WAITING entry (FIFO).
  //   mode 1 (read-priority): first READ entry; writes only if no reads.
  //   mode 2 (write-drain): read-priority normally; while a write window is
  //     active, first WRITE entry; reads only if no writes.
  bool drain_active = (m_scheduler == 2 && m_write_drain_until > 0);
  bool prefer_reads =
      (m_scheduler == 1) || (m_scheduler == 2 && !drain_active);

  for (unsigned issued = 0; issued < m_max_active; issued++) {
    if (active_count >= m_max_active) break;
    auto pick = m_mshr_queue.end();
    // Pass 1: preferred type
    for (auto qit = m_mshr_queue.begin(); qit != m_mshr_queue.end(); ++qit) {
      auto mit = m_mshr.find(*qit);
      if (mit == m_mshr.end() || mit->second.op_state != OP_WAITING) continue;
      bool is_read = !mit->second.pending.front()->get_is_write();
      if (is_read == prefer_reads) { pick = qit; break; }
    }
    // Pass 2: any type (avoids starvation when the preferred type is absent)
    if (pick == m_mshr_queue.end()) {
      for (auto qit = m_mshr_queue.begin(); qit != m_mshr_queue.end(); ++qit) {
        auto mit = m_mshr.find(*qit);
        if (mit != m_mshr.end() && mit->second.op_state == OP_WAITING) {
          pick = qit; break;
        }
      }
    }
    if (pick == m_mshr_queue.end()) break;
    auto it = pick;
    auto mshr_it = m_mshr.find(*it);
    if (drain_active && mshr_it != m_mshr.end() &&
        !mshr_it->second.pending.front()->get_is_write()) {
      n_read_deferred_in_window++;  // read held by a write window
    }

    // Channel affinity (OCP §4.5): an operation can only execute on a
    // sub-array of its OWN channel. There is deliberately no "any idle
    // sub-array" fallback: serving from another channel's die set would
    // break channel resource isolation, and with the FTL enabled it could
    // execute a write at a different physical location than the mapping
    // table records (a correctness hazard). If the tag is somehow outside
    // the channel's range, clamp it into the channel's set.
    unsigned sa_id = mshr_it->second.subarray_id;
    hbf_channel_t *ch = m_channels[mshr_it->second.channel_id];
    if (!ch->owns_subarray(sa_id)) {
      sa_id = ch->subarray_of_local(sa_id);
      mshr_it->second.subarray_id = sa_id;
    }
    if (!m_subarrays[sa_id]->is_idle()) { ++it; continue; }

    hbf_subarray_t *sa = m_subarrays[sa_id];
    mem_fetch *first_mf = mshr_it->second.pending.front();

    if (first_mf->get_is_write()) {
      // ═══ Write path: erase-before-write ═══
      // v0.4: Invalidate page cache (old data is stale)
      if (m_page_cache->enabled()) {
        m_page_cache->invalidate(mshr_it->second.phys_page);
      }
      if (mshr_it->second.needs_erase) {
        // Block not erased → issue ERASE first
        if (m_ftl) m_ftl->mark_block_erasing(sa_id, mshr_it->second.block_id);
        sa->start_erase(mshr_it->second.block_id);
        mshr_it->second.op_state = OP_ERASING;
        mshr_it->second.issue_cycle = m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle;
        n_block_erases++;
        ch->n_block_erases++;
      } else {
        // Block erased → issue PROGRAM
        sa->start_program(mshr_it->second.page_offset,
                          mshr_it->second.block_id);
        mshr_it->second.op_state = OP_PROGRAMMING;
        mshr_it->second.issue_cycle = m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle;
        n_page_programs++;
        ch->n_page_programs++;
      }
    } else {
      // ═══ Read path: page cache lookup ═══
      if (m_page_cache->lookup(mshr_it->second.phys_page)) {
        // Cache hit — fast completion, no subarray needed
        mshr_it->second.cache_hit_ready_cycle =
            m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle +
            m_config->hbf_cache_hit_latency;
        mshr_it->second.op_state = OP_WAITING;
        mshr_it->second.issue_cycle = m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle;
        n_cache_hits++;
      } else {
        // Cache miss — normal subarray read
        sa->start_read(mshr_it->second.page_offset, mshr_it->second.block_id);
        mshr_it->second.op_state = OP_READING;
        mshr_it->second.issue_cycle = m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle;
        n_page_reads++;
        ch->n_page_reads++;
        n_cache_misses++;
      }
    }

    m_mshr_queue.erase(it);
    if (mshr_it->second.op_state != OP_WAITING) {
      // Issued a real NAND op (READ/PROGRAM/ERASE) — track the sub-array as
      // active so cycle() only ticks busy sub-arrays.
      m_active_subarrays.insert(sa_id);
      active_count++;
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
  fprintf(simFile, "HBF DRAM Clock:         %.1f MHz (all HBF timers count DRAM-clock ticks)\n",
          m_dram_freq_mhz);
  fprintf(simFile, "HBF Sub-arrays:         %u\n", m_num_subarrays);
  fprintf(simFile, "HBF Page Reads:         %llu\n", n_page_reads);
  fprintf(simFile, "HBF Page Programs:      %llu\n", n_page_programs);
  fprintf(simFile, "HBF Block Erases:       %llu\n", n_block_erases);
  fprintf(simFile, "HBF Partial Page Programs: %llu (write amplification source, OCP 5.4.1)\n",
          n_partial_page_programs);
  fprintf(simFile, "HBF Write Timeout Flushes: %llu\n", n_write_timeout_flushes);
  fprintf(simFile, "HBF Idle Drain Flushes:    %llu\n", n_idle_drain_flushes);
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
    // End-to-end latencies are timestamped in GPU core cycles; NAND op
    // timers (tR/tPROG/tBERS) count DRAM-clock ticks. Report both, each
    // converted with its own clock frequency.
    double us_per_core_cycle = 1000.0 / m_core_freq_mhz;
    double us_per_dram_tick  = 1000.0 / m_dram_freq_mhz;
    fprintf(simFile,
            "HBF Avg Op Latency:     %llu core cycles (%.2f us)\n",
            total_lat / total_ops,
            (total_lat / total_ops) * us_per_core_cycle);
    fprintf(simFile,
            "HBF NAND Timing:        tR=%u ticks (%.2f us)  tPROG=%u ticks (%.2f us)  tBERS=%u ticks (%.2f us)\n",
            m_config->hbf_tR, m_config->hbf_tR * us_per_dram_tick,
            m_config->hbf_tPROG, m_config->hbf_tPROG * us_per_dram_tick,
            m_config->hbf_tBERS, m_config->hbf_tBERS * us_per_dram_tick);
  }
  fprintf(simFile, "HBF Max Queue Depth:    %u\n", max_queue_depth);
  fprintf(simFile, "HBF Cycles Active:      %llu\n", n_cycles_active);

  // Per-channel summary (v0.4, OCP §4.5): requests, NAND ops, outstanding
  // writes (limit: %u), write deferrals, and link-stall ticks.
  fprintf(simFile, "HBF Channels:           %u (map mode %s, per-channel BW %.0f GB/s, write limit %u)\n",
          m_num_channels,
          m_channel_map_mode == 1 ? "partition" : "interleave",
          m_config->hbf_channel_bw_gbps,
          m_config->hbf_max_outstanding_writes);
  if (n_read_requests > 0)
    fprintf(simFile, "HBF Read Latency:       avg=%llu max=%llu core cycles (%llu reqs)\n",
            total_read_latency / n_read_requests, max_read_latency,
            n_read_requests);
  fprintf(simFile, "HBF Scheduler:          %s (drain windows=%llu, reads deferred in windows=%llu)\n",
          m_scheduler == 0 ? "FCFS" : (m_scheduler == 1 ? "read-priority" : "write-drain"),
          n_write_drain_windows, n_read_deferred_in_window);
  for (unsigned c = 0; c < m_num_channels; c++) {
    hbf_channel_t *ch = m_channels[c];
    fprintf(simFile,
            "  HBF Ch%2u: subarrays=[%u,%u) reqs=%llu rd=%llu pg=%llu er=%llu ow=%u def=%llu stall=%llu\n",
            c, ch->get_first_subarray(),
            ch->get_first_subarray() + ch->get_num_subarrays(),
            ch->n_requests, ch->n_page_reads, ch->n_page_programs,
            ch->n_block_erases, ch->n_outstanding_writes,
            ch->n_writes_deferred, ch->n_transfer_stall_ticks);
  }

  if (m_ftl) {
    m_ftl->print_stat(simFile);
  }

  // Per-subarray summary
  unsigned idle_count = 0;
  unsigned long long total_buffer_hits = 0, total_buffer_misses = 0;
  for (unsigned i = 0; i < m_num_subarrays; i++) {
    if (m_subarrays[i]->is_idle()) idle_count++;
    total_buffer_hits += m_subarrays[i]->n_buffer_hits;
    total_buffer_misses += m_subarrays[i]->n_buffer_misses;
  }
  fprintf(simFile, "HBF Active Sub-arrays:  %u / %u\n",
          m_num_subarrays - idle_count, m_num_subarrays);
  unsigned long long total_buffer_accesses = total_buffer_hits + total_buffer_misses;
  if (total_buffer_accesses > 0) {
    fprintf(simFile, "HBF Page Buffer Hits:   %llu (%.1f%%)\n",
            total_buffer_hits,
            100.0 * total_buffer_hits / total_buffer_accesses);
  }

  // v0.4: Page cache statistics
  fprintf(simFile, "HBF Page Cache:         %s (%u entries, hits=%llu misses=%llu)\n",
          m_page_cache->enabled() ? "enabled" : "disabled",
          m_page_cache->size(),
          m_page_cache->hits,
          m_page_cache->misses);
  if (m_page_cache->hits + m_page_cache->misses > 0) {
    fprintf(simFile, "HBF Page Cache Hit Rate: %.1f%%  Evictions: %llu\n",
            100.0 * m_page_cache->hits /
                (m_page_cache->hits + m_page_cache->misses),
            m_page_cache->evictions);
  }

  fprintf(simFile, "==========================================================\n\n");
}
