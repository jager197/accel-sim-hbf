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
#include <stdlib.h>
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
  m_num_subarrays = config->hbf_num_subarrays > 0 ? config->hbf_num_subarrays : 1;
  m_max_active    = config->hbf_max_active > 0 ? config->hbf_max_active : 1;

  // All HBF timing counters count DRAM-clock ticks (hbf_cycle runs in the
  // DRAM clock domain, exactly like dram_t). Record the clock frequencies
  // in MHz for statistics reporting (ticks -> us) and documentation.
  // NOTE: gpgpu_sim_config stores frequencies in Hz (the MhZ macro
  // multiplies by 1e6), so divide by 1e6 here.
  m_dram_freq_mhz = gpu->get_config().get_dram_freq() / 1e6;
  m_core_freq_mhz = gpu->get_config().get_core_freq() / 1e6;
  if (m_dram_freq_mhz <= 0.0) m_dram_freq_mhz = 1.0;
  if (m_core_freq_mhz <= 0.0) m_core_freq_mhz = 1.0;
  m_hbf_tick = 0;

  // ── Host channels (v0.4) ────────────────────────────────────────────
  // Each channel owns a contiguous slice of the cube-wide sub-array array.
  // hbf_num_channels is the total count for this logical cube, not a
  // per-memory-partition count. Configuration normalization happens in
  // memory_config::init(); retain a defensive clamp for malformed values.
  m_num_channels = config->hbf_num_channels;
  if (m_num_channels == 0) m_num_channels = 1;
  if (m_num_channels > 16 && !config->hbf_allow_non_ocp_channels)
    m_num_channels = 16;
  if (m_num_channels > m_num_subarrays) m_num_channels = m_num_subarrays;
  // placement_mode=2 uses the optional table; mode 0 keeps the legacy
  // hbf_channel_map switch so old experiment configs remain reproducible.
  m_channel_map_mode = (config->hbf_placement_mode == 1 ||
                        (config->hbf_placement_mode == 0 &&
                         config->hbf_channel_map == 1))
                           ? 1
                           : 0;
  m_explicit_placement = config->hbf_placement_mode == 2;
  if (m_explicit_placement) {
    std::string error;
    if (!hbf_load_placement_table(config->hbf_channel_map_file,
                                  m_num_channels,
                                  &m_explicit_channel_map, &error)) {
      fail_fast(error.c_str(), ~0ull);
    }
  }
  unsigned long long total_pages =
      config->hbf_page_size ? config->hbf_size / config->hbf_page_size : 0;
  // ceil division keeps partition mapping well-defined for small spans.
  m_pages_per_channel =
      (total_pages + m_num_channels - 1) / m_num_channels;
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

  // Allocate only the pointer table. Sub-array state and page-buffer storage
  // are materialized on first access, so a large configured capacity does not
  // impose per-die host allocation on workloads that never touch HBF.
  m_subarrays = new hbf_subarray_t *[m_num_subarrays];
  for (unsigned i = 0; i < m_num_subarrays; i++) m_subarrays[i] = NULL;

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

  m_read_mode = config->hbf_read_mode == 1 ? 1 : 0;
  m_read_agg_window = config->hbf_read_agg_window;
  m_read_agg_threshold = config->hbf_read_agg_threshold > 0
                             ? config->hbf_read_agg_threshold
                             : config->hbf_page_size;
  m_write_timeout_policy = config->hbf_write_timeout_policy == 1 ? 1 : 0;
  m_trace = new hbf_trace_t(config->hbf_trace_file, config->hbf_trace_level);

  // Scheduling policy (v0.4): 0=FCFS, 1=read-priority, 2=write-drain
  m_scheduler = config->hbf_scheduler;
  if (m_scheduler < 0 || m_scheduler > 2) m_scheduler = 0;
  m_write_drain_high = config->hbf_write_drain_high > 0
                           ? config->hbf_write_drain_high
                           : 1;
  m_write_drain_until = 0;
  m_write_drain_read_due = false;
  n_write_drain_windows = 0;
  n_read_deferred_in_window = 0;
  n_write_drain_read_breaks = 0;

  // Logic-die page cache (0 entries = disabled; not CUDA Shared Memory).
  m_page_cache = new hbf_page_cache_t(m_config->hbf_cache_entries);

  // Statistics
  n_page_reads       = 0;
  n_page_programs    = 0;
  n_block_erases     = 0;
  n_mshr_hits        = 0;
  n_total_requests   = 0;
  n_diag_read_wait_ticks = 0;
  n_diag_read_program_wait_ticks = 0;
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
  n_incomplete_page_errors = 0;
  n_write_coverage_errors = 0;
  n_mapping_errors = 0;
  n_host_write_bytes = 0;
  n_unique_write_bytes = 0;
  n_media_program_bytes = 0;
  n_padding_bytes = 0;
  n_requested_read_bytes = 0;
  n_media_read_bytes = 0;
  n_read_aggregation_wait = 0;
  n_read_aggregation_pages = 0;
  n_first_request_latency = 0;
  n_first_request_pages = 0;
  n_read_requests = 0;
  n_write_requests = 0;
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
  delete m_trace;
}

hbf_subarray_t *hbf_controller_t::ensure_subarray(unsigned subarray_id) {
  assert(subarray_id < m_num_subarrays);
  if (m_subarrays[subarray_id] == NULL)
    m_subarrays[subarray_id] = new hbf_subarray_t(subarray_id, m_config);
  return m_subarrays[subarray_id];
}

// ============================================================================
// Address to page translation
// ============================================================================
unsigned long long hbf_controller_t::addr_to_page(new_addr_type addr) const {
  if (m_config->hbf_page_size == 0) return 0;
  unsigned long long relative =
      addr >= m_config->hbf_base_addr ? addr - m_config->hbf_base_addr : addr;
  return relative / m_config->hbf_page_size;
}

unsigned hbf_controller_t::channel_of_page(unsigned long long page) const {
  if (m_explicit_placement) {
    auto it = m_explicit_channel_map.find(page);
    if (it == m_explicit_channel_map.end())
      fail_fast("requested page is absent from explicit placement table", page);
    if (it->second >= m_num_channels)
      fail_fast("explicit placement channel is out of range", page);
    return it->second;
  }
  if (m_num_channels <= 1) return 0;
  return hbf_page_to_channel(m_num_channels, m_channel_map_mode, page,
                             m_pages_per_channel);
}

unsigned hbf_controller_t::subarray_of_page(unsigned long long page,
                                            unsigned channel) const {
  unsigned long long local_page = page;
  if (!m_explicit_placement) {
    if (m_channel_map_mode == 1) {
      local_page = page - (unsigned long long)channel * m_pages_per_channel;
    } else {
      local_page = page / m_num_channels;
    }
  }
  return m_channels[channel]->subarray_of_local(local_page);
}

void hbf_controller_t::direct_physical_address(
    unsigned long long page, unsigned channel, unsigned *subarray,
    unsigned *block, unsigned *page_offset) const {
  unsigned pages_per_block = m_config->hbf_pages_per_block;
  if (!hbf_direct_page_identity(page, pages_per_block, block, page_offset))
    fail_fast("logical page exceeds direct-map physical namespace", page);
  *subarray = subarray_of_page(page, channel);
}

void hbf_controller_t::fail_fast(const char *reason,
                                 unsigned long long page) const {
  if (page == ~0ull) {
    fprintf(stderr, "HBF fatal: partition=%u: %s\n", m_id, reason);
  } else {
    fprintf(stderr, "HBF fatal: partition=%u page=%llu: %s\n", m_id, page,
            reason);
  }
  fflush(stderr);
  exit(EXIT_FAILURE);
}

bool hbf_controller_t::add_write_coverage(write_buffer_entry_t *entry,
                                          const mem_fetch *data) {
  if (entry == NULL || data == NULL || m_config->hbf_page_size == 0)
    return false;

  new_addr_type addr = data->get_addr();
  unsigned long long relative =
      addr >= m_config->hbf_base_addr ? addr - m_config->hbf_base_addr : addr;
  unsigned long long offset = relative % m_config->hbf_page_size;
  unsigned new_bytes = 0;
  mem_access_byte_mask_t mask = data->get_access_byte_mask();
  bool valid = false;
  if (mask.any()) {
    unsigned long long mask_base =
        relative - relative % MAX_MEMORY_ACCESS_SIZE;
    unsigned long long mask_page = mask_base / m_config->hbf_page_size;
    valid = mask_page == entry->page_addr &&
            entry->coverage.add_mask(mask_base % m_config->hbf_page_size,
                                     mask, &new_bytes);
  } else {
    valid = entry->coverage.add(offset, data->get_data_size(), &new_bytes);
  }
  if (!valid) {
    n_write_coverage_errors++;
    return false;
  }
  n_unique_write_bytes += new_bytes;
  return true;
}

// ============================================================================
// Backpressure check
// ============================================================================
bool hbf_controller_t::full(bool is_write) const {
  unsigned max_q = m_config->hbf_max_outstanding;
  if (max_q == 0) return false;  // unlimited
  // The limit applies to unique page operations in the logical cube. Buffered
  // pages are included so a full MSHR cannot be hidden behind an unbounded
  // write buffer.
  return m_mshr.size() + m_write_buffer.size() >= max_q;
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
  unsigned long long current_tick = m_hbf_tick;

  n_total_requests++;

  unsigned long long page_addr = addr_to_page(data->get_addr());
  unsigned ch = channel_of_page(page_addr);
  m_channels[ch]->n_requests++;
  if (m_trace != NULL)
    m_trace->record(current_cycle, "INGRESS", data, page_addr, ch, 0,
                    data->get_data_size(), (unsigned)que_length(), 0, false,
                    false, "");

  // Writes: buffer first, then flush to MSHR
  if (data->get_is_write()) {
    mem_access_byte_mask_t mask = data->get_access_byte_mask();
    unsigned host_bytes = hbf_effective_write_bytes(mask, data->get_data_size());
    auto wb_it = m_write_buffer.find(page_addr);
    if (wb_it != m_write_buffer.end()) {
      // Coalesce into existing write buffer entry
      wb_it->second.requests.push_back(data);
      wb_it->second.last_arrival_tick = current_tick;
      if (!add_write_coverage(&wb_it->second, data))
        fail_fast("write byte coverage crosses a NAND page", page_addr);
      n_host_write_bytes += host_bytes;
      if (wb_it->second.coverage.full())
        flush_write_buffer_entry(page_addr, false);
      return;
    }
    // New write buffer entry
    write_buffer_entry_t wb = {};
    wb.page_addr = page_addr;
    wb.first_arrival_cycle = current_cycle;
    wb.first_arrival_tick = current_tick;
    wb.last_arrival_tick = current_tick;
    wb.coverage.reset(m_config->hbf_page_size);
    wb.requests.push_back(data);
    if (!add_write_coverage(&wb, data))
      fail_fast("write byte coverage crosses a NAND page", page_addr);
    n_host_write_bytes += host_bytes;
    m_write_buffer[page_addr] = wb;

    // A complete page can leave the aggregation buffer immediately. If its
    // channel is saturated, flush_write_buffer_entry keeps it buffered.
    if (m_write_buffer[page_addr].coverage.full()) {
      flush_write_buffer_entry(page_addr, false);
      return;
    }

    // Flush if buffer is full
    if (m_write_buffer.size() >= m_write_buffer_max) {
      flush_write_buffer();
    }
    return;
  }

  n_requested_read_bytes += data->get_data_size();

  // Reads: flush any buffered writes to same page first (read-after-write)
  auto wb_it = m_write_buffer.find(page_addr);
  if (wb_it != m_write_buffer.end()) {
    flush_write_buffer_entry(wb_it->first, true);
    wb_it = m_write_buffer.find(page_addr);
    if (wb_it != m_write_buffer.end()) {
      // If an earlier write for this page is already in flight, attach the
      // read to that entry. This preserves read-after-write ordering even
      // when the channel outstanding-write limit deferred the flush.
      for (auto &pending_entry : m_mshr) {
        if (pending_entry.second.phys_page != page_addr ||
            pending_entry.second.pending.empty())
          continue;
        if (pending_entry.second.pending.front()->get_is_write()) {
          pending_entry.second.pending.push_back(data);
          pending_entry.second.requested_bytes += data->get_data_size();
          n_mshr_hits++;
          return;
        }
      }
    }
  }

  // Check MSHR (only if enabled)
  if (m_config->hbf_mshr_enabled) {
    auto it = m_mshr.find(page_addr);
    if (it != m_mshr.end()) {
      it->second.pending.push_back(data);
      it->second.requested_bytes += data->get_data_size();
      if (m_read_mode == 1 && it->second.read_aggregation_pending &&
          it->second.requested_bytes >= m_read_agg_threshold) {
        n_read_aggregation_pages++;
        n_read_aggregation_wait += current_tick - it->second.first_arrival_tick;
        it->second.read_aggregation_pending = false;
      }
      n_mshr_hits++;
      return;
    }
  }

  // Create MSHR entry for read
  unsigned long long entry_key = m_config->hbf_mshr_enabled
                                     ? page_addr
                                     : (page_addr << 20) | n_total_requests;
  mshr_entry_t entry = {};
  entry.phys_page   = page_addr;
  entry.channel_id  = ch;
  entry.op_state    = OP_WAITING;
  entry.delivery_pending = false;
  entry.fill_cache_on_deliver = false;
  entry.read_aggregation_pending = (m_read_mode == 1);
  entry.is_write_operation = false;
  entry.first_delivery_recorded = false;
  entry.cache_hit_ready_tick = 0;
  entry.first_arrival_cycle = current_cycle;
  entry.first_arrival_tick = current_tick;
  entry.requested_bytes = data->get_data_size();
  entry.pending.push_back(data);
  if (m_read_mode == 1 && entry.requested_bytes >= m_read_agg_threshold) {
    n_read_aggregation_pages++;
    entry.read_aggregation_pending = false;
  }

  if (m_ftl) {
    hbf_phys_addr_t phys = m_ftl->translate(page_addr, false);
    if (!phys.valid() || phys.subarray >= m_num_subarrays ||
        !m_channels[ch]->owns_subarray(phys.subarray)) {
      n_mapping_errors++;
      fail_fast("FTL read mapping violates channel/subarray affinity",
                page_addr);
    } else {
      entry.subarray_id = phys.subarray;
      entry.block_id    = phys.block;
      entry.page_offset = phys.page;
    }
  } else {
    direct_physical_address(page_addr, ch, &entry.subarray_id,
                            &entry.block_id, &entry.page_offset);
  }

  m_mshr[entry_key] = entry;
  m_mshr_queue.push_back(entry_key);
  if (m_mshr_queue.size() > max_queue_depth) max_queue_depth = m_mshr_queue.size();
}

// Flush all buffered writes to MSHR and return the number actually accepted.
unsigned hbf_controller_t::flush_write_buffer() {
  std::vector<unsigned long long> keys;
  for (auto &kv : m_write_buffer) keys.push_back(kv.first);
  unsigned flushed = 0;
  for (auto key : keys) {
    if (flush_write_buffer_entry(key, false)) ++flushed;
  }
  // Deferred entries remain buffered until channel/MSHR capacity is
  // available. Clearing the map here would silently drop those writes.
  return flushed;
}

// Flush a single write buffer entry to MSHR.
// `force` bypasses the per-channel outstanding-write limit — used by the
// read-after-write path, where the write MUST reach the MSHR so the read
// coalesces onto it (ordering). Regular flushes (buffer full / timeout)
// defer when the channel is saturated (OCP §5.4.1.7 product limit).
bool hbf_controller_t::flush_write_buffer_entry(unsigned long long page_addr,
                                                bool force) {
  auto it = m_write_buffer.find(page_addr);
  if (it == m_write_buffer.end()) return false;
  write_buffer_entry_t &wb = it->second;
  if (wb.requests.empty()) {
    m_write_buffer.erase(it);
    return false;
  }

  unsigned ch = channel_of_page(wb.page_addr);
  if (!force &&
      !m_channels[ch]->can_accept_write(m_config->hbf_max_outstanding_writes)) {
    m_channels[ch]->n_writes_deferred++;
    return false;  // channel saturated — keep buffering
  }

  // Per-page serialization: if an MSHR entry for this page is already in
  // flight (a read or an earlier write), do NOT create a second entry —
  // the MSHR is keyed by page address and a second entry would overwrite
  // the first, silently losing its pending requests. Keep the data in the
  // write buffer and retry on a later cycle (the timeout/idle loops retry
  // every cycle until the in-flight entry completes).
  if (m_mshr.find(wb.page_addr) != m_mshr.end()) {
    m_channels[ch]->n_writes_deferred++;
    return false;
  }

  unsigned long long wb_page_addr = wb.page_addr;
  unsigned long long entry_key = m_config->hbf_mshr_enabled
                                     ? wb_page_addr
                                     : (wb_page_addr << 20) | n_total_requests;

  mshr_entry_t entry = {};
  entry.phys_page   = page_addr;
  entry.channel_id  = ch;
  entry.op_state    = OP_WAITING;
  entry.delivery_pending = false;
  entry.fill_cache_on_deliver = false;
  entry.read_aggregation_pending = false;
  entry.is_write_operation = true;
  entry.first_delivery_recorded = false;
  entry.cache_hit_ready_tick = 0;
  entry.first_arrival_cycle = wb.first_arrival_cycle;
  entry.first_arrival_tick = wb.first_arrival_tick;
  entry.requested_bytes = 0;
  entry.pending     = std::move(wb.requests);
  // Count partial-page programs: OCP §5.4.1 requires a FULL 4KiB page to be
  // accumulated before programming (the Base die controller aggregates
  // complete 4KiB). A flush with less data is a partial-page program — a
  // write-amplification source that the paper quantifies.
  for (mem_fetch *mf : entry.pending)
    entry.requested_bytes += mf->get_data_size();
  entry.write_covered_bytes = wb.coverage.covered_bytes();
  if (entry.write_covered_bytes < m_config->hbf_page_size &&
      m_write_timeout_policy == 1) {
    n_incomplete_page_errors++;
    fail_fast("strict write policy rejected an incomplete NAND page",
              page_addr);
  }

  if (m_ftl) {
    hbf_phys_addr_t phys = m_ftl->translate(page_addr, true);
    if (!phys.valid()) {
      if (m_ftl->mapping_error()) {
        n_mapping_errors++;
        fail_fast("FTL write page is absent from explicit placement table",
                  page_addr);
      }
      fail_fast("FTL capacity exhausted while allocating write page",
                page_addr);
    }
    if (phys.subarray >= m_num_subarrays ||
        !m_channels[ch]->owns_subarray(phys.subarray)) {
      n_mapping_errors++;
      fail_fast("FTL write mapping violates channel/subarray affinity",
                page_addr);
    }
    entry.subarray_id = phys.subarray;
    entry.block_id    = phys.block;
    entry.page_offset = phys.page;
  } else {
    direct_physical_address(page_addr, ch, &entry.subarray_id,
                            &entry.block_id, &entry.page_offset);
  }

  m_channels[ch]->n_outstanding_writes++;
  m_mshr[entry_key] = entry;
  m_mshr_queue.push_back(entry_key);
  if (m_mshr_queue.size() > max_queue_depth) max_queue_depth = m_mshr_queue.size();
  // Remove the drained entry from the write buffer so later writes to the
  // same page create a FRESH buffer entry (a stale empty entry would be
  // silently reused, batching new writes into a page whose MSHR entry is
  // already in flight — the root cause of the double-flush/lost-request
  // bug in v0.4.0).
  m_write_buffer.erase(it);
  return true;
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
  unsigned long long current_tick = ++m_hbf_tick;

  if (!m_mshr_queue.empty() || m_mshr.size() > 0 || !m_write_buffer.empty()) {
    n_cycles_active++;
  }

  // Observation only: do not instantiate resources or alter scheduling order.
  // Count read request-ticks queued behind an already executing PROGRAM.
  if (m_trace != NULL && m_trace->diagnostics_enabled()) {
    for (const auto &key : m_mshr_queue) {
      auto entry = m_mshr.find(key);
      if (entry == m_mshr.end() || entry->second.op_state != OP_WAITING)
        continue;
      unsigned reads = 0;
      for (const auto *request : entry->second.pending)
        if (!request->get_is_write()) ++reads;
      n_diag_read_wait_ticks += reads;
      unsigned sa = entry->second.subarray_id;
      if (sa < m_num_subarrays && m_subarrays[sa] != NULL &&
          m_subarrays[sa]->get_state() == hbf_subarray_t::PROGRAMMING)
        n_diag_read_program_wait_ticks += reads;
    }
  }

  // Release aggregation-mode reads once the bounded window or byte
  // threshold is met.  The first demand request is never delayed in mode 0.
  if (m_read_mode == 1) {
    for (auto &kv : m_mshr) {
      mshr_entry_t &entry = kv.second;
      if (!entry.read_aggregation_pending || entry.pending.empty() ||
          entry.pending.front()->get_is_write())
        continue;
      bool threshold = entry.requested_bytes >= m_read_agg_threshold;
      bool window = m_read_agg_window == 0 ||
                    current_tick - entry.first_arrival_tick >=
                        m_read_agg_window;
      if (threshold || window) {
        entry.read_aggregation_pending = false;
        n_read_aggregation_pages++;
        n_read_aggregation_wait += current_tick - entry.first_arrival_tick;
      }
    }
  }

  // Replenish per-channel interface transfer credits (UCIe/AXI link BW,
  // OCP §4.2 Table 2: per-channel raw BW x AXI link efficiency).
  for (unsigned c = 0; c < m_num_channels; c++) {
    m_channels[c]->replenish_credit();
  }

  // Count channel activity once per tick, even when several sub-arrays in a
  // channel are active concurrently.  This reports resource occupancy rather
  // than multiplying activity by the number of memory partitions.
  for (unsigned c = 0; c < m_num_channels; c++) {
    bool active = false;
    for (unsigned sa_id : m_active_subarrays) {
      if (m_channels[c]->owns_subarray(sa_id)) {
        active = true;
        break;
      }
    }
    if (active) m_channels[c]->n_active_ticks++;
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
            kv.second.op_state = OP_WAITING;
            m_mshr_queue.push_back(kv.first);
          } else {
            // Read/program completion: mark for incremental delivery.
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

  // Cache hits use the same incremental delivery path as completed NAND ops.
  for (auto &kv : m_mshr) {
    if (!kv.second.delivery_pending &&
        kv.second.cache_hit_ready_tick != 0 &&
        current_tick >= kv.second.cache_hit_ready_tick) {
      kv.second.delivery_pending = true;
    }
  }

  // Step 2: Deliver as many completed requests as the return queue and link
  // permit. Requiring room for the whole coalesced page deadlocks whenever
  // pending.size() is larger than the return queue capacity.
  for (auto it = m_mshr.begin(); it != m_mshr.end();) {
    if (!it->second.delivery_pending) { ++it; continue; }
    unsigned queue_length = m_returnq->get_length();
    unsigned queue_capacity = m_returnq->get_max_len();
    if (queue_length >= queue_capacity) { ++it; continue; }
    unsigned delivery_limit = hbf_delivery_batch_limit(
        (unsigned)it->second.pending.size(), queue_length, queue_capacity);

    hbf_channel_t *ch = m_channels[it->second.channel_id];
    if (it->second.fill_cache_on_deliver && m_page_cache->enabled()) {
      m_page_cache->insert(it->second.phys_page);
      it->second.fill_cache_on_deliver = false;
    }

    unsigned long long latency = current_cycle - it->second.issue_cycle;
    unsigned delivered = 0;
    bool credit_stalled = false;
    while (delivered < delivery_limit) {
      mem_fetch *mf = it->second.pending[delivered];
      unsigned transfer_bytes = mf->get_is_write() ? 0 : mf->get_data_size();
      if (transfer_bytes > 0 && !ch->try_consume_credit(transfer_bytes)) {
        credit_stalled = true;
        break;
      }
      ch->n_transfer_bytes += transfer_bytes;
      if (mf->get_access_type() != L1_WRBK_ACC &&
          mf->get_access_type() != L2_WRBK_ACC) {
        mf->set_reply();
      }
      m_returnq->push(mf);
      if (m_trace != NULL)
        m_trace->record(current_cycle, "COMPLETED", mf, it->second.phys_page,
                        it->second.channel_id, it->second.subarray_id,
                        mf->get_data_size(), (unsigned)m_returnq->get_length(),
                        latency, it->second.cache_hit_ready_tick != 0, false,
                        "");
      if (mf->get_is_write()) {
        total_write_latency += latency;
        n_write_requests++;
        n_bytes_written += hbf_effective_write_bytes(
            mf->get_access_byte_mask(), mf->get_data_size());
      } else {
        total_read_latency += latency; n_bytes_read += mf->get_data_size();
        n_read_requests++;
        if (latency > max_read_latency) max_read_latency = latency;
      }
      ++delivered;
    }
    if (credit_stalled) ch->n_transfer_stall_ticks++;
    if (delivered > 0) {
      if (!it->second.first_delivery_recorded) {
        n_first_request_latency +=
            current_cycle - it->second.first_arrival_cycle;
        n_first_request_pages++;
        it->second.first_delivery_recorded = true;
      }
      it->second.pending.erase(it->second.pending.begin(),
                               it->second.pending.begin() + delivered);
    }
    if (it->second.pending.empty()) {
      if (it->second.is_write_operation && ch->n_outstanding_writes > 0) {
        ch->n_outstanding_writes--;
      }
      it = m_mshr.erase(it);
    } else {
      ++it;
    }
  }

  // Step 3a: FTL housekeeping (GC state machine)
  if (m_ftl) {
    m_ftl->cycle();
  }

  // Step 3a2: write-drain window lifecycle (v0.4, scheduler mode 2).
  // A window opens when PENDING writes (buffered entries plus queued but
  // unscheduled MSHR write entries -- timeout flushes move writes out of
  // the buffer, so buffer occupancy alone underestimates pressure) reach
  // the high-water mark, and lasts at most hbf_write_drain_maxwait ticks.
  // Sustained pressure cannot immediately reopen an expired window: one
  // schedulable read must issue first, making the configured bound effective.
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

    hbf_write_drain_event_t event = hbf_update_write_drain(
        pending_writes, m_write_drain_high, current_tick,
        m_config->hbf_write_drain_maxwait, &m_write_drain_until,
        &m_write_drain_read_due);
    if (event == HBF_DRAIN_OPENED)
      n_write_drain_windows++;
  }

  // Step 3b: Flush write buffer entries that have been waiting too long.
  // The deadline is host-configured (OCP §5.4.1.6). Partial mode preserves
  // legacy workload behavior; strict mode terminates on an incomplete page.
  unsigned timeout = m_config->hbf_write_agg_timeout;
  auto wb_it = m_write_buffer.begin();
  while (wb_it != m_write_buffer.end()) {
    if (current_tick - wb_it->second.first_arrival_tick > timeout) {
      unsigned long long page = (wb_it++)->first;
      if (flush_write_buffer_entry(page, false)) n_write_timeout_flushes++;
    } else {
      ++wb_it;
    }
  }

  // Step 4: Schedule new operations
  schedule_operations();

  // Step 5: Idle drain (v0.4) — when the device has nothing in flight and
  // no write data has arrived for a quiet period, flush buffered pages
  // so tail writes are not silently dropped at kernel end. The quiet period
  // (100 ticks) preserves 4KiB write aggregation: a flush right after the
  // FIRST store of a page would split the page into a partial program and
  // re-open the double-flush hazard (v0.4.0 bug).
  if (!m_write_buffer.empty() && m_mshr.empty() && m_mshr_queue.empty()) {
    unsigned long long last_arrival = 0;
    for (const auto &kv : m_write_buffer)
      if (kv.second.last_arrival_tick > last_arrival)
        last_arrival = kv.second.last_arrival_tick;
    if (current_tick - last_arrival > 100) {
      if (flush_write_buffer() > 0) n_idle_drain_flushes++;
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

    std::vector<std::list<unsigned long long>::iterator> candidate_iters;
    std::vector<hbf_schedule_candidate_t> candidates;
    for (auto qit = m_mshr_queue.begin(); qit != m_mshr_queue.end(); ++qit) {
      auto mit = m_mshr.find(*qit);
      hbf_schedule_candidate_t candidate = {};
      candidate.waiting =
          mit != m_mshr.end() && mit->second.op_state == OP_WAITING &&
          !mit->second.pending.empty();
      if (candidate.waiting) {
        mshr_entry_t &entry = mit->second;
        if (entry.channel_id >= m_num_channels ||
            entry.subarray_id >= m_num_subarrays ||
            !m_channels[entry.channel_id]->owns_subarray(entry.subarray_id)) {
          n_mapping_errors++;
          fail_fast("queued mapping violates channel/subarray affinity",
                    entry.phys_page);
        }
        candidate.aggregation_pending = entry.read_aggregation_pending;
        candidate.subarray_idle = ensure_subarray(entry.subarray_id)->is_idle();
        candidate.is_read = !entry.pending.front()->get_is_write();
      }
      candidate_iters.push_back(qit);
      candidates.push_back(candidate);
    }

    int selected = hbf_pick_schedule_candidate(
        candidates, prefer_reads, m_scheduler != 0);
    if (selected < 0) break;
    if (m_scheduler == 2 && m_write_drain_read_due &&
        candidates[(size_t)selected].is_read) {
      m_write_drain_read_due = false;
      n_write_drain_read_breaks++;
    }
    n_read_deferred_in_window +=
        hbf_count_deferred_reads(candidates, selected, drain_active);
    auto pick = candidate_iters[(size_t)selected];
    auto it = pick;
    auto mshr_it = m_mshr.find(*it);

    // Channel affinity (OCP §4.5): an operation can only execute on a
    // sub-array of its OWN channel. Translation validates this invariant and
    // turns violations into terminal errors; the scheduler never rewrites a
    // physical mapping to make it appear valid.
    unsigned sa_id = mshr_it->second.subarray_id;
    hbf_channel_t *ch = m_channels[mshr_it->second.channel_id];
    assert(sa_id < m_num_subarrays);
    assert(ch->owns_subarray(sa_id));
    assert(m_subarrays[sa_id] != NULL && m_subarrays[sa_id]->is_idle());

    hbf_subarray_t *sa = m_subarrays[sa_id];
    mem_fetch *first_mf = mshr_it->second.pending.front();

    if (first_mf->get_is_write()) {
      // ═══ Write path: erase-before-write ═══
      // v0.4: Invalidate page cache (old data is stale)
      if (m_page_cache->enabled()) {
        m_page_cache->invalidate(mshr_it->second.phys_page);
      }
      // Query block state at issue time. Several pages can reserve offsets in
      // one recycled block before its first ERASE; after that ERASE completes,
      // every waiter observes the updated state and programs without another.
      if (m_ftl &&
          !m_ftl->is_block_erased(sa_id, mshr_it->second.block_id)) {
        // Block not erased → issue ERASE first
        m_ftl->mark_block_erasing(sa_id, mshr_it->second.block_id);
        sa->start_erase(mshr_it->second.block_id);
        mshr_it->second.op_state = OP_ERASING;
        mshr_it->second.issue_cycle = m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle;
        n_block_erases++;
        ch->n_block_erases++;
        if (m_trace != NULL)
          m_trace->record(mshr_it->second.issue_cycle, "ERASE", first_mf,
                          mshr_it->second.phys_page,
                          mshr_it->second.channel_id, sa_id, 0,
                          (unsigned)m_mshr_queue.size(), 0, false, false, "");
      } else {
        // Block erased → issue PROGRAM
        sa->start_program(mshr_it->second.page_offset,
                          mshr_it->second.block_id);
        mshr_it->second.op_state = OP_PROGRAMMING;
        mshr_it->second.issue_cycle = m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle;
        n_page_programs++;
        n_media_program_bytes += m_config->hbf_page_size;
        unsigned padding = hbf_write_padding_bytes(
            m_config->hbf_page_size, mshr_it->second.write_covered_bytes);
        if (padding > 0) {
          n_partial_page_programs++;
          n_padding_bytes += padding;
        }
        ch->n_page_programs++;
        if (m_trace != NULL)
          m_trace->record(mshr_it->second.issue_cycle, "PROGRAM", first_mf,
                          mshr_it->second.phys_page,
                          mshr_it->second.channel_id, sa_id,
                          m_config->hbf_page_size,
                          (unsigned)m_mshr_queue.size(), 0, false, false, "");
      }
    } else {
      // ═══ Read path: page cache lookup ═══
      if (m_page_cache->lookup(mshr_it->second.phys_page)) {
        // Cache hit — fast completion, no subarray needed
        mshr_it->second.cache_hit_ready_tick =
            m_hbf_tick + m_config->hbf_cache_hit_latency;
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
        n_media_read_bytes += m_config->hbf_page_size;
        if (m_trace != NULL)
          m_trace->record(mshr_it->second.issue_cycle, "READ", first_mf,
                          mshr_it->second.phys_page,
                          mshr_it->second.channel_id, sa_id,
                          m_config->hbf_page_size,
                          (unsigned)m_mshr_queue.size(), 0, false, false, "");
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
  return (unsigned)(m_mshr_queue.size() + m_mshr.size() +
                    m_write_buffer.size() + m_returnq->get_length());
}

bool hbf_controller_t::has_work() const {
  return !m_mshr.empty() || !m_mshr_queue.empty() ||
         !m_write_buffer.empty() || !m_active_subarrays.empty() ||
         m_returnq->get_length() != 0;
}

// ============================================================================
// Statistics printing
// ============================================================================
void hbf_controller_t::print_stat(FILE *simFile) {
  fprintf(simFile, "\n========= HBF Cube Controller Statistics =========\n");
  fprintf(simFile, "HBF DRAM Clock:         %.1f MHz (all HBF timers count DRAM-clock ticks)\n",
          m_dram_freq_mhz);
  fprintf(simFile, "HBF Sub-arrays:         %u\n", m_num_subarrays);
  fprintf(simFile, "HBF Logical Cube Channels: %u (global namespace)\n",
          m_num_channels);
  fprintf(simFile, "HBF Page Reads:         %llu\n", n_page_reads);
  fprintf(simFile, "HBF Page Programs:      %llu\n", n_page_programs);
  fprintf(simFile, "HBF Block Erases:       %llu\n", n_block_erases);
  fprintf(simFile, "HBF Partial Page Programs: %llu (compatibility policy)\n",
          n_partial_page_programs);
  fprintf(simFile, "HBF Incomplete Page Errors: %llu (strict policy)\n",
          n_incomplete_page_errors);
  fprintf(simFile, "HBF Write Coverage Errors: %llu\n",
          n_write_coverage_errors);
  fprintf(simFile, "HBF Mapping Errors:        %llu\n", n_mapping_errors);
  fprintf(simFile, "HBF Host Write Bytes:      %llu\n", n_host_write_bytes);
  fprintf(simFile, "HBF Unique Write Bytes:    %llu\n", n_unique_write_bytes);
  fprintf(simFile, "HBF Media Program Bytes:   %llu\n", n_media_program_bytes);
  fprintf(simFile, "HBF Padding Bytes:       %llu\n", n_padding_bytes);
  fprintf(simFile, "HBF Write Traffic Ratio:  %.4f\n",
          n_host_write_bytes > 0
              ? (double)n_media_program_bytes / n_host_write_bytes
              : 0.0);
  fprintf(simFile, "HBF Write Timeout Flushes: %llu (successful entries)\n",
          n_write_timeout_flushes);
  fprintf(simFile, "HBF Idle Drain Events:     %llu (at least one entry flushed)\n",
          n_idle_drain_flushes);
  fprintf(simFile, "HBF Total Requests:     %llu\n", n_total_requests);
  fprintf(simFile, "HBF MSHR Hits:          %llu (%.1f%% coalesced)\n",
          n_mshr_hits,
          n_total_requests > 0
              ? 100.0 * n_mshr_hits / n_total_requests
              : 0.0);
  fprintf(simFile, "HBF Bytes Read:         %llu\n", n_bytes_read);
  fprintf(simFile, "HBF Bytes Written:      %llu\n", n_bytes_written);
  fprintf(simFile, "HBF Read Policy:        %s (window=%u ticks threshold=%u bytes)\n",
          m_read_mode == 0 ? "demand" : "aggregation", m_read_agg_window,
          m_read_agg_threshold);
  fprintf(simFile, "HBF Requested Read Bytes: %llu\n", n_requested_read_bytes);
  fprintf(simFile, "HBF Media Read Bytes:     %llu\n", n_media_read_bytes);
  fprintf(simFile, "HBF Read Amplification:   %.4f\n",
          n_requested_read_bytes > 0
              ? (double)n_media_read_bytes / n_requested_read_bytes
              : 0.0);
  fprintf(simFile, "HBF Aggregation Wait:      %llu ticks\n",
          n_read_aggregation_wait);
  fprintf(simFile, "HBF First-Request Latency: %llu core cycles (%llu pages)\n",
          n_first_request_latency, n_first_request_pages);
  if (n_read_requests + n_write_requests > 0) {
    unsigned long long total_lat = total_read_latency + total_write_latency;
    unsigned long long total_requests = n_read_requests + n_write_requests;
    // End-to-end latencies are timestamped in GPU core cycles; NAND op
    // timers (tR/tPROG/tBERS) count DRAM-clock ticks. Report both, each
    // converted with its own clock frequency.
    // MHz is cycles per microsecond; its reciprocal is us per cycle.
    double us_per_core_cycle = 1.0 / m_core_freq_mhz;
    double us_per_dram_tick  = 1.0 / m_dram_freq_mhz;
    fprintf(simFile,
            "HBF Avg Op Latency:     %llu core cycles (%.2f us)\n",
            total_lat / total_requests,
            (total_lat / total_requests) * us_per_core_cycle);
    fprintf(simFile,
            "HBF NAND Timing:        tR=%u ticks (%.2f us)  tPROG=%u ticks (%.2f us)  tBERS=%u ticks (%.2f us)\n",
            m_config->hbf_tR, m_config->hbf_tR * us_per_dram_tick,
            m_config->hbf_tPROG, m_config->hbf_tPROG * us_per_dram_tick,
            m_config->hbf_tBERS, m_config->hbf_tBERS * us_per_dram_tick);
  }
  if (m_trace != NULL && m_trace->diagnostics_enabled()) {
    fprintf(simFile, "HBF Diagnostic Read Queue Request Ticks: %llu\n",
            n_diag_read_wait_ticks);
    fprintf(simFile, "HBF Diagnostic Read PROGRAM Request Ticks: %llu\n",
            n_diag_read_program_wait_ticks);
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
  unsigned long long sim_cycles =
      m_gpu->gpu_sim_cycle + m_gpu->gpu_tot_sim_cycle;
  double min_channel_util = 1.0e30;
  double max_channel_util = 0.0;
  double sum_channel_util = 0.0;
  if (n_read_requests > 0)
    fprintf(simFile, "HBF Read Latency:       avg=%llu max=%llu core cycles (%llu reqs)\n",
            total_read_latency / n_read_requests, max_read_latency,
            n_read_requests);
  fprintf(simFile, "HBF Scheduler:          %s (drain windows=%llu, reads deferred in windows=%llu)\n",
          m_scheduler == 0 ? "FCFS" : (m_scheduler == 1 ? "read-priority" : "write-drain"),
          n_write_drain_windows, n_read_deferred_in_window);
  fprintf(simFile, "HBF Drain Read Breaks:  %llu\n",
          n_write_drain_read_breaks);
  for (unsigned c = 0; c < m_num_channels; c++) {
    hbf_channel_t *ch = m_channels[c];
    fprintf(simFile,
            "  HBF Ch%2u: subarrays=[%u,%u) reqs=%llu rd=%llu pg=%llu er=%llu ow=%u def_attempts=%llu stall=%llu\n",
            c, ch->get_first_subarray(),
            ch->get_first_subarray() + ch->get_num_subarrays(),
            ch->n_requests, ch->n_page_reads, ch->n_page_programs,
            ch->n_block_erases, ch->n_outstanding_writes,
            ch->n_writes_deferred, ch->n_transfer_stall_ticks);
    double util = sim_cycles > 0
                      ? 100.0 * (double)ch->n_active_ticks / sim_cycles
                      : 0.0;
    fprintf(simFile,
            "    active_ticks=%llu util=%.3f%% transfer_bytes=%llu\n",
            ch->n_active_ticks, util, ch->n_transfer_bytes);
    if (util < min_channel_util) min_channel_util = util;
    if (util > max_channel_util) max_channel_util = util;
    sum_channel_util += util;
  }
  if (m_num_channels > 0) {
    double avg = sum_channel_util / m_num_channels;
    double imbalance = avg > 0.0 ? (max_channel_util - min_channel_util) / avg
                                 : 0.0;
    fprintf(simFile,
            "HBF Channel Utilization: min=%.3f%% avg=%.3f%% max=%.3f%% imbalance=%.4f\n",
            min_channel_util, avg, max_channel_util, imbalance);
  }

  if (m_ftl) {
    m_ftl->print_stat(simFile);
  }

  // Per-subarray summary
  unsigned idle_count = 0;
  unsigned long long total_buffer_hits = 0, total_buffer_misses = 0;
  for (unsigned i = 0; i < m_num_subarrays; i++) {
    if (m_subarrays[i] == NULL || m_subarrays[i]->is_idle()) idle_count++;
    if (m_subarrays[i] != NULL) {
      total_buffer_hits += m_subarrays[i]->n_buffer_hits;
      total_buffer_misses += m_subarrays[i]->n_buffer_misses;
    }
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
