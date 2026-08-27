// HBF Phase 2: Simple Page-Level FTL Implementation

#include "hbf_ftl.h"
#include "gpu-sim.h"
#include "hbf_channel.h"
#include <algorithm>

namespace {
// Channel-aware preferred sub-array for a logical page (v0.4): allocation
// must land inside the page's own host channel (OCP §4.5 — a channel can
// only use its own die set). Mirrors hbf_controller_t::channel_of_page and
// clamps identically (channel count in [1,16], <= sub-array count).
unsigned hbf_ftl_channel_of_page(const memory_config *cfg,
                                 unsigned long long page) {
  unsigned nch = cfg->hbf_num_channels;
  if (nch == 0) nch = 1;
  if (nch > 16) nch = 16;
  if (nch > cfg->hbf_num_subarrays) nch = cfg->hbf_num_subarrays;
  if (nch <= 1) return 0;
  unsigned long long pages_per_ch =
      (cfg->hbf_size / cfg->hbf_page_size) / nch;
  if (cfg->hbf_channel_map == 1) {
    unsigned ch = (unsigned)(page / pages_per_ch);
    return ch < nch ? ch : nch - 1;
  }
  return (unsigned)(page % nch);
}

// Preferred (sub-array, channel-local spread) for a logical page: a stable
// sub-array inside the page's channel, so channel interleaving spreads
// pages across the channel's die set.
unsigned hbf_ftl_preferred_subarray(const memory_config *cfg,
                                    unsigned long long page) {
  unsigned nch = cfg->hbf_num_channels;
  if (nch == 0) nch = 1;
  if (nch > 16) nch = 16;
  if (nch > cfg->hbf_num_subarrays) nch = cfg->hbf_num_subarrays;
  unsigned per_ch = cfg->hbf_num_subarrays / nch;
  unsigned ch = hbf_ftl_channel_of_page(cfg, page);
  return ch * per_ch + (unsigned)(page % per_ch);
}
}  // namespace

hbf_ftl_t::hbf_ftl_t(const memory_config *config)
    : m_config(config),
      m_has_active_block(false),
      m_gc_active(false),
      m_gc_cycles_remaining(0),
      m_gc_victim_subarray(0),
      m_next_block_id(0),
      m_max_blocks(0),
      n_capacity_exceeded(0),
      m_media_mode(config->hbf_media_mode != 1 ? 0 : 1),
      m_blocks_per_zone(config->hbf_blocks_per_zone > 0
                            ? config->hbf_blocks_per_zone
                            : 64),
      m_zone_remap_enabled(config->hbf_zone_remap_enabled),
      m_zone_remap_threshold(config->hbf_zone_remap_threshold),
      n_zone_remaps(0) {
  n_translations    = 0;
  n_allocations     = 0;
  n_gcs             = 0;
  n_gc_page_copies  = 0;
  n_gc_block_erases = 0;
  n_gc_stall_cycles = 0;
  n_wl_biased_allocs = 0;
  n_wl_biased_gcs    = 0;

  // ── Capacity enforcement (v0.4) ─────────────────────────────────────
  // The configured HBF capacity (hbf_size, e.g. 512 GiB) is shared by all
  // memory partitions (one HBF controller per partition). Each partition's
  // FTL may therefore create at most:
  //     max_blocks = (hbf_size / n_mem) / (pages_per_block * page_size)
  // This replaces the previous unbounded block creation, which modeled an
  // infinite-capacity device (hbf_size was only used for address routing).
  unsigned long long bytes_per_partition =
      m_config->hbf_size / (m_config->m_n_mem > 0 ? m_config->m_n_mem : 1);
  unsigned long long bytes_per_block =
      (unsigned long long)m_config->hbf_pages_per_block *
      m_config->hbf_page_size;
  m_max_blocks = bytes_per_partition / bytes_per_block;
}

// ============================================================================
// translate() — logical page → physical address
// ============================================================================
hbf_phys_addr_t hbf_ftl_t::translate(unsigned long long logical_page,
                                     bool is_write) {
  n_translations++;

  auto it = m_mapping.find(logical_page);
  if (it != m_mapping.end()) {
    // Existing mapping: for reads, return it; for writes, invalidate old
    if (!is_write) {
      return it->second;
    }
    // Write: invalidate old mapping, allocate new page
    invalidate(logical_page);
  } else if (!is_write) {
    // Reads of never-written pages do NOT allocate (FTL semantics — a read
    // never creates a mapping). Direct-map to a stable pseudo-physical
    // location so the timing model still has a sub-array to schedule on.
    // The pseudo location lies inside the page's own channel (v0.4).
    // Without this, read-only streaming workloads fill every block and
    // trigger GC on every new block (O(n^2) churn / multi-hour hangs).
    hbf_phys_addr_t phys;
    phys.subarray = hbf_ftl_preferred_subarray(m_config, logical_page);
    phys.block    = 0;
    phys.page     = 0;
    return phys;
  }

  // Need to allocate: find a block with free pages
  if (!m_has_active_block) {
    m_active_block = allocate_block(hbf_ftl_preferred_subarray(m_config, logical_page));
    m_has_active_block = true;
  }

  hbf_ftl_t::block_key_t &bk = m_active_block;
  auto blk_it = m_blocks.find(bk);
  assert(blk_it != m_blocks.end());

  unsigned page = blk_it->second.next_free_page;
  blk_it->second.next_free_page++;
  blk_it->second.free_pages--;
  blk_it->second.valid_pages++;

  // If this block is full, need a new one next time
  if (blk_it->second.free_pages == 0) {
    m_has_active_block = false;
  }

  hbf_phys_addr_t phys(bk.subarray, bk.block, page);
  m_mapping[logical_page] = phys;

  // Maintain reverse index: (subarray, block) → logical pages
  m_reverse_map[bk].insert(logical_page);

  n_allocations++;

  return phys;
}

// ============================================================================
// invalidate() — mark a logical page no longer valid
// ============================================================================
void hbf_ftl_t::invalidate(unsigned long long logical_page) {
  auto it = m_mapping.find(logical_page);
  if (it == m_mapping.end()) return;

  hbf_phys_addr_t &phys = it->second;
  hbf_ftl_t::block_key_t bk;
  bk.subarray = phys.subarray;
  bk.block    = phys.block;

  auto blk_it = m_blocks.find(bk);
  if (blk_it != m_blocks.end() && blk_it->second.valid_pages > 0) {
    blk_it->second.valid_pages--;
    // HBF mode (OCP §11.4): no GC and no device-side data movement — a
    // block whose last valid page is gone is simply released back to the
    // free pool (it will be erased on reuse). SSD mode keeps the block
    // until GC reclaims it.
    if (m_media_mode == 0 && blk_it->second.valid_pages == 0 &&
        !m_free_blocks.count(bk)) {
      m_free_blocks.insert(bk);
    }
  }

  // Maintain reverse index
  auto rev_it = m_reverse_map.find(bk);
  if (rev_it != m_reverse_map.end()) {
    rev_it->second.erase(logical_page);
    if (rev_it->second.empty()) {
      m_reverse_map.erase(rev_it);
    }
  }

  m_mapping.erase(it);
}

// ============================================================================
// needs_gc() — check if GC is needed (free blocks below threshold)
// ============================================================================
bool hbf_ftl_t::needs_gc() const {
  // HBF mode (OCP §11.4): the device NEVER runs garbage collection. The
  // over-provisioning/GC machinery is an SSD-ism; in HBF mode blocks are
  // released on full invalidation instead.
  if (m_media_mode == 0) return false;
  unsigned total_blocks = m_blocks.size();
  if (total_blocks == 0) return false;

  unsigned free_blocks = m_free_blocks.size();
  float free_ratio = (float)free_blocks / total_blocks;
  return free_ratio < m_config->hbf_overprovisioning;
}

// ============================================================================
// gc() — GREEDY: pick the block with fewest valid pages, remap them, erase
//
// v0.3: Properly remaps valid logical pages in the victim block to new
// physical pages, fixing the stale-mapping bug. Models GC latency as
// valid_pages × tPROG cycles (the PROGRAM cost of relocating data).
// Victim block erase latency (tBERS) is covered by the existing
// erase-before-write path when the recycled block is first reused.
// ============================================================================
void hbf_ftl_t::gc() {
  if (!needs_gc()) return;

  // Find victim: block with lowest cost (GREEDY or wear-aware)
  hbf_ftl_t::block_key_t victim;
  unsigned min_valid = ~0u;
  double min_cost = 1e30;
  bool found = false;

  // Precompute average erase count for wear-aware cost function
  double avg_erase = 0.0;
  if (m_config->hbf_wear_leveling_enabled && !m_blocks.empty()) {
    for (auto &kv : m_blocks) avg_erase += kv.second.erase_count;
    avg_erase /= m_blocks.size();
  }

  for (auto &kv : m_blocks) {
    const hbf_ftl_t::block_key_t &bk = kv.first;
    const block_info_t &info = kv.second;

    // Skip free blocks and the active allocation block
    if (m_free_blocks.count(bk)) continue;
    if (m_has_active_block && bk.subarray == m_active_block.subarray &&
        bk.block == m_active_block.block)
      continue;
    // Handle blocks with 0 valid pages: just erase them (no copying needed)
    if (info.valid_pages == 0) {
      m_free_blocks.insert(bk);
      continue;
    }

    if (m_config->hbf_wear_leveling_enabled && avg_erase > 0) {
      // Wear-aware cost: valid_pages penalised for above-average wear.
      // Hot blocks (high erase_count) get higher cost → less likely victim,
      // spreading wear across the array.
      double wear_penalty = 1.0;
      if (info.erase_count > avg_erase) {
        wear_penalty += 0.5 * (info.erase_count - avg_erase) / avg_erase;
      }
      double cost = info.valid_pages * wear_penalty;
      if (cost < min_cost) {
        min_cost = cost;
        victim = bk;
        found = true;
        min_valid = info.valid_pages;  // keep for stats
        n_wl_biased_gcs++;
      }
    } else {
      // Pure GREEDY (default, or when wear leveling disabled)
      if (info.valid_pages < min_valid) {
        min_valid = info.valid_pages;
        victim = bk;
        found = true;
      }
    }
  }

  if (!found) return;

  // ── Step 1: Remap valid logical pages from victim to new blocks ──
  block_info_t &v_info = m_blocks[victim];
  unsigned pages_relocated = 0;
  unsigned victim_subarray = victim.subarray;

  // Look up which logical pages reside in the victim block via reverse index
  auto rev_it = m_reverse_map.find(victim);
  if (rev_it != m_reverse_map.end()) {
    // Copy the set since we will modify it during iteration
    std::vector<unsigned long long> pages_to_move(
        rev_it->second.begin(), rev_it->second.end());

    for (unsigned long long lp : pages_to_move) {
      // Verify this logical page is still mapped to the victim
      auto map_it = m_mapping.find(lp);
      if (map_it == m_mapping.end()) continue;
      if (map_it->second.subarray != victim.subarray ||
          map_it->second.block != victim.block)
        continue;

      // Allocate a new physical page for GC relocation
      // (no recursive GC trigger — uses internal allocation path)
      hbf_phys_addr_t new_phys = allocate_page_for_gc(lp);

      // Update forward mapping: logical page → new physical location
      m_mapping[lp] = new_phys;

      // Update reverse index: remove from victim, add to target block
      hbf_ftl_t::block_key_t new_bk;
      new_bk.subarray = new_phys.subarray;
      new_bk.block    = new_phys.block;
      m_reverse_map[new_bk].insert(lp);

      pages_relocated++;
    }

    // Clean up: remove reverse-index entry for victim block
    m_reverse_map.erase(victim);
  }

  // ── Step 2: Erase victim block and return to free pool ──
  v_info.erased = true;
  v_info.free_pages = v_info.total_pages;
  v_info.next_free_page = 0;
  v_info.valid_pages = 0;
  m_free_blocks.insert(victim);

  // ── Step 3: Model GC latency ──
  // Each relocated page costs one tPROG cycle on the target subarray.
  // The victim block erase (tBERS) is NOT added here — it is covered
  // by the existing erase-before-write path when the recycled block
  // is first reused by a future write.
  if (pages_relocated > 0) {
    m_gc_active = true;
    m_gc_cycles_remaining = pages_relocated * m_config->hbf_tPROG;
    m_gc_victim_subarray = victim_subarray;
  }

  // ── Step 4: Statistics ──
  n_gc_page_copies += pages_relocated;
  n_gcs++;
  n_gc_block_erases++;
}

// ============================================================================
// cycle() — per-cycle FTL housekeeping (GC state machine)
// ============================================================================
void hbf_ftl_t::cycle() {
  if (m_gc_active) {
    if (m_gc_cycles_remaining > 0) {
      m_gc_cycles_remaining--;
      n_gc_stall_cycles++;
    }
    if (m_gc_cycles_remaining == 0) {
      m_gc_active = false;
    }
  }
  // HBF mode: host-controlled zone remapping accounting (OCP §11.4.1).
  if (m_media_mode == 0 && m_zone_remap_enabled) {
    maybe_zone_remap();
  }
}

// ============================================================================
// maybe_zone_remap() — host-controlled wear leveling accounting (OCP §11.4.1)
//
// The host divides each channel's capacity into equal-size zones and swaps a
// Hot Zone (high PEC) with a Cold Zone (low PEC) to spread wear. HBF itself
// never moves data. In this sprint-level model the remap is *accounted*
// (event count + per-zone PEC spread in stats), not executed: there is no
// per-zone data movement because the model keeps no zone-level data
// placement. When the PEC spread between the hottest and coldest zone
// exceeds the configured threshold, a remap event is counted.
// ============================================================================
void hbf_ftl_t::maybe_zone_remap() {
  if (m_blocks.empty()) return;
  // Per-zone PEC: zone = block_id / blocks_per_zone.
  std::map<unsigned, unsigned long long> zone_pec;
  for (auto &kv : m_blocks) {
    unsigned zone = kv.first.block / m_blocks_per_zone;
    zone_pec[zone] += kv.second.erase_count;
  }
  if (zone_pec.size() < 2) return;
  unsigned long long min_pec = ~0ull, max_pec = 0;
  for (auto &zv : zone_pec) {
    if (zv.second < min_pec) min_pec = zv.second;
    if (zv.second > max_pec) max_pec = zv.second;
  }
  if (max_pec - min_pec > m_zone_remap_threshold) {
    n_zone_remaps++;
  }
}

// ============================================================================
// allocate_page_for_gc() — allocate a single page without triggering GC
//
// Extracts the core page-allocation logic from translate() for use by
// gc() during victim-page relocation. Uses a separate code path to avoid
// recursive GC (allocate_block → needs_gc → gc → allocate_page_for_gc).
// ============================================================================
hbf_phys_addr_t hbf_ftl_t::allocate_page_for_gc(
    unsigned long long logical_page) {
  // Ensure we have an active block with free pages
  if (!m_has_active_block) {
    // Allocate from free pool if available, otherwise create new block
    // (but do NOT call allocate_block() which may trigger recursive gc())
    if (!m_free_blocks.empty()) {
      m_active_block = *m_free_blocks.begin();
      m_free_blocks.erase(m_free_blocks.begin());

      block_info_t &info = m_blocks[m_active_block];
      info.free_pages = info.total_pages;
      info.next_free_page = 0;
      info.valid_pages = 0;
      info.erased = false;
    } else {
      // Create a brand-new block (last resort)
      m_active_block.subarray = hbf_ftl_preferred_subarray(m_config, logical_page);
      m_active_block.block = m_next_block_id++;

      block_info_t info;
      info.total_pages    = m_config->hbf_pages_per_block;
      info.free_pages     = m_config->hbf_pages_per_block;
      info.valid_pages    = 0;
      info.next_free_page = 0;
      info.erased         = false;
      info.erase_count    = 0;
      m_blocks[m_active_block] = info;
    }
    m_has_active_block = true;
  }

  hbf_ftl_t::block_key_t &bk = m_active_block;
  auto blk_it = m_blocks.find(bk);
  assert(blk_it != m_blocks.end());

  unsigned page = blk_it->second.next_free_page;
  blk_it->second.next_free_page++;
  blk_it->second.free_pages--;
  blk_it->second.valid_pages++;

  if (blk_it->second.free_pages == 0) {
    m_has_active_block = false;
  }

  hbf_phys_addr_t phys(bk.subarray, bk.block, page);
  n_allocations++;
  return phys;
}

// (end of allocate_page_for_gc)

// ============================================================================
// allocate_block() — find or create a block for new writes
// ============================================================================
hbf_ftl_t::block_key_t hbf_ftl_t::allocate_block(unsigned preferred_subarray) {
  // Try free block pool first
  if (!m_free_blocks.empty()) {
    hbf_ftl_t::block_key_t bk;
    if (m_config->hbf_wear_leveling_enabled) {
      // Wear-aware: pick the block with the lowest erase count
      unsigned long long min_erase = ~0ull;
      for (auto &fb : m_free_blocks) {
        auto it = m_blocks.find(fb);
        if (it != m_blocks.end() && it->second.erase_count < min_erase) {
          min_erase = it->second.erase_count;
          bk = fb;
        }
      }
      n_wl_biased_allocs++;
    } else {
      bk = *m_free_blocks.begin();
    }
    m_free_blocks.erase(bk);

    // Reset block info (preserve erase_count for wear tracking)
    block_info_t &info = m_blocks[bk];
    info.free_pages = info.total_pages;
    info.next_free_page = 0;
    info.valid_pages = 0;
    info.erased = false;
    return bk;
  }

  // Check if GC can free up blocks
  if (needs_gc()) {
    gc();
    if (!m_free_blocks.empty()) {
      return allocate_block(preferred_subarray);
    }
  }

  // ── Capacity enforcement (v0.4) ─────────────────────────────────────
  // The device is bounded: never create a block beyond m_max_blocks.
  // On exhaustion, recycle the block with the fewest valid pages (the
  // "capacity overflow" path — loudly counted in stats). It is unreachable
  // for the paper workloads (working sets are orders of magnitude below
  // the 512 GiB configuration), but makes the model bounded and lets
  // capacity-utilization claims be meaningful.
  if (m_blocks.size() >= m_max_blocks) {
    n_capacity_exceeded++;
    block_key_t victim;
    unsigned min_valid = ~0u;
    bool found = false;
    for (auto &kv : m_blocks) {
      if (m_free_blocks.count(kv.first)) continue;
      if (kv.second.valid_pages < min_valid) {
        min_valid = kv.second.valid_pages;
        victim = kv.first;
        found = true;
      }
    }
    if (!found && m_has_active_block) {
      victim = m_active_block;
      found = true;
    }
    if (found) {
      // Discard the victim's valid mappings (counted as capacity overflow).
      auto rev_it = m_reverse_map.find(victim);
      if (rev_it != m_reverse_map.end()) {
        for (unsigned long long lp : rev_it->second) {
          auto map_it = m_mapping.find(lp);
          if (map_it != m_mapping.end() &&
              map_it->second.subarray == victim.subarray &&
              map_it->second.block == victim.block) {
            m_mapping.erase(map_it);
          }
        }
        m_reverse_map.erase(rev_it);
      }
      block_info_t &info = m_blocks[victim];
      info.free_pages = info.total_pages;
      info.next_free_page = 0;
      info.valid_pages = 0;
      info.erased = false;  // erase-before-write erases it before programming
      if (m_has_active_block && victim.subarray == m_active_block.subarray &&
          victim.block == m_active_block.block) {
        m_has_active_block = false;
      }
      return victim;
    }
    // No block at all (m_blocks empty): let the new-block path below run.
  }

  // Create a new block
  hbf_ftl_t::block_key_t bk;
  bk.subarray = preferred_subarray;
  bk.block    = m_next_block_id++;

  block_info_t info;
  info.total_pages   = m_config->hbf_pages_per_block;
  info.free_pages    = m_config->hbf_pages_per_block;
  info.valid_pages   = 0;
  info.next_free_page = 0;
  info.erased         = false;
  info.erase_count    = 0;
  m_blocks[bk] = info;

  return bk;
}

// ============================================================================
// Block erase state tracking
// ============================================================================
bool hbf_ftl_t::is_block_erased(unsigned subarray, unsigned block) const {
  hbf_ftl_t::block_key_t bk;
  bk.subarray = subarray;
  bk.block    = block;
  auto it = m_blocks.find(bk);
  if (it == m_blocks.end()) return false;
  return it->second.erased;
}

void hbf_ftl_t::mark_block_erasing(unsigned subarray, unsigned block) {
  hbf_ftl_t::block_key_t bk;
  bk.subarray = subarray;
  bk.block    = block;
  m_blocks[bk].erased = false;  // block is being erased, not yet ready
}

void hbf_ftl_t::mark_block_erased(unsigned subarray, unsigned block) {
  hbf_ftl_t::block_key_t bk;
  bk.subarray = subarray;
  bk.block    = block;
  m_blocks[bk].erased = true;
  m_blocks[bk].free_pages = m_blocks[bk].total_pages;
  m_blocks[bk].next_free_page = 0;
  m_blocks[bk].erase_count++;  // physical erase completed → increment wear
}

// ============================================================================
// get_usage() — compute capacity usage summary
// ============================================================================
hbf_usage_info_t hbf_ftl_t::get_usage() const {
  hbf_usage_info_t u;
  u.total_pages = 0;
  u.valid_pages = 0;
  u.free_pages = 0;
  u.total_blocks = m_blocks.size();
  u.free_blocks = m_free_blocks.size();
  u.logical_pages = m_mapping.size();
  u.subarrays_used = 0;
  u.subarrays_total = m_config->hbf_num_subarrays;
  for (unsigned i = 0; i < 5; i++) u.histogram[i] = 0;

  // Per-subarray accumulation: subarray → (valid pages, total pages)
  std::map<unsigned, std::pair<unsigned long long, unsigned long long>> per_sa;

  for (auto &kv : m_blocks) {
    const block_info_t &info = kv.second;
    u.total_pages += info.total_pages;
    u.valid_pages += info.valid_pages;
    u.free_pages += info.free_pages;
    per_sa[kv.first.subarray].first += info.valid_pages;
    per_sa[kv.first.subarray].second += info.total_pages;
  }

  u.subarrays_used = per_sa.size();
  for (auto &kv : per_sa) {
    double frac = kv.second.second > 0
                      ? (double)kv.second.first / kv.second.second
                      : 0.0;
    int bucket = (kv.second.first == 0)   ? 0
                 : (frac <= 0.25)         ? 1
                 : (frac <= 0.50)         ? 2
                 : (frac <= 0.75)         ? 3
                                          : 4;
    u.histogram[bucket]++;
  }

  return u;
}

// ============================================================================
// Statistics printing
// ============================================================================
void hbf_ftl_t::print_stat(FILE *fp) const {
  fprintf(fp, "HBF FTL Translations:   %llu\n", n_translations);
  fprintf(fp, "HBF FTL Allocations:    %llu\n", n_allocations);
  fprintf(fp, "HBF FTL GC Events:      %llu\n", n_gcs);
  fprintf(fp, "HBF FTL GC Page Copies: %llu\n", n_gc_page_copies);
  fprintf(fp, "HBF FTL GC Erases:      %llu\n", n_gc_block_erases);
  fprintf(fp, "HBF FTL GC Stall Cycles:%llu\n", n_gc_stall_cycles);
  if (n_gcs > 0) {
    fprintf(fp, "HBF FTL Avg GC Latency: %llu cycles\n",
            n_gc_stall_cycles / n_gcs);
  }
  fprintf(fp, "HBF FTL Total Blocks:   %zu\n", m_blocks.size());
  fprintf(fp, "HBF FTL Free Blocks:    %zu\n", m_free_blocks.size());
  fprintf(fp, "HBF FTL Capacity:       %llu / %llu max blocks (%.2f GiB modeled per partition)\n",
          m_blocks.size(), m_max_blocks,
          (double)m_max_blocks * m_config->hbf_pages_per_block *
              m_config->hbf_page_size / (1024.0 * 1024 * 1024));
  fprintf(fp, "HBF FTL Cap Exceeded:   %llu\n", n_capacity_exceeded);
  fprintf(fp, "HBF FTL Media Mode:     %s (OCP %s; GC %s)\n",
          m_media_mode == 0 ? "hbf" : "ssd",
          m_media_mode == 0 ? "11.4: no GC, zone-based, no valid-data movement"
                            : "SSD semantics (comparison baseline)",
          m_media_mode == 0 ? "disabled" : "enabled");
  if (m_media_mode == 0 && !m_blocks.empty()) {
    // Per-zone PEC spread (host-controlled wear leveling, OCP 11.4.1).
    std::map<unsigned, unsigned long long> zone_pec;
    for (auto &kv : m_blocks) {
      unsigned zone = kv.first.block / m_blocks_per_zone;
      zone_pec[zone] += kv.second.erase_count;
    }
    unsigned long long min_pec = ~0ull, max_pec = 0;
    for (auto &zv : zone_pec) {
      if (zv.second < min_pec) min_pec = zv.second;
      if (zv.second > max_pec) max_pec = zv.second;
    }
    fprintf(fp, "HBF FTL Zones:          %zu zones x %u blocks, PEC spread min=%llu max=%llu, remaps=%llu (threshold %u)\n",
            zone_pec.size(), m_blocks_per_zone, min_pec, max_pec,
            n_zone_remaps, m_zone_remap_threshold);
  }

  // Capacity usage — what fraction of HBF is occupied.
  // Used pages include live data only; freed pages (GC'd or invalidated)
  // are reusable capacity that the FTL hands back on demand.
  hbf_usage_info_t u = get_usage();
  double util = u.total_pages > 0 ? 100.0 * u.valid_pages / u.total_pages : 0.0;
  fprintf(fp, "HBF Capacity Usage:    %llu / %llu pages used (%.2f%%)\n",
          u.valid_pages, u.total_pages, util);
  fprintf(fp, "HBF Capacity Bytes:    %llu / %llu bytes used\n",
          u.valid_pages * m_config->hbf_page_size,
          u.total_pages * m_config->hbf_page_size);
  fprintf(fp, "HBF Logical Pages:     %llu live mappings\n", u.logical_pages);
  fprintf(fp, "HBF Subarray Spread:   %u / %u subarrays hold data\n",
          u.subarrays_used, u.subarrays_total);
  fprintf(fp, "HBF Subarray Usage:    empty=%u low=%u mid=%u high=%u full=%u\n",
          u.histogram[0], u.histogram[1], u.histogram[2], u.histogram[3],
          u.histogram[4]);

  // Wear leveling statistics
  if (!m_blocks.empty()) {
    unsigned long long min_erase = ~0ull, max_erase = 0, sum_erase = 0;
    for (auto &kv : m_blocks) {
      unsigned long long ec = kv.second.erase_count;
      if (ec < min_erase) min_erase = ec;
      if (ec > max_erase) max_erase = ec;
      sum_erase += ec;
    }
    fprintf(fp, "HBF FTL Wear Leveling:  %s\n",
            m_config->hbf_wear_leveling_enabled ? "enabled" : "disabled");
    fprintf(fp, "HBF FTL Erase Count:    min=%llu avg=%llu max=%llu\n",
            min_erase, sum_erase / m_blocks.size(), max_erase);
    fprintf(fp, "HBF FTL WL Allocs:      %llu\n", n_wl_biased_allocs);
    fprintf(fp, "HBF FTL WL GCs:         %llu\n", n_wl_biased_gcs);
  }
}
