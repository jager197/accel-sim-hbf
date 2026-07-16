// HBF Phase 2: Simple Page-Level FTL Implementation

#include "hbf_ftl.h"
#include "gpu-sim.h"
#include <algorithm>

hbf_ftl_t::hbf_ftl_t(const memory_config *config)
    : m_config(config),
      m_has_active_block(false),
      m_gc_active(false),
      m_gc_cycles_remaining(0),
      m_gc_victim_subarray(0),
      m_next_block_id(0) {
  n_translations    = 0;
  n_allocations     = 0;
  n_gcs             = 0;
  n_gc_page_copies  = 0;
  n_gc_block_erases = 0;
  n_gc_stall_cycles = 0;
  n_wl_biased_allocs = 0;
  n_wl_biased_gcs    = 0;
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
  }

  // Need to allocate: find a block with free pages
  if (!m_has_active_block) {
    m_active_block = allocate_block(logical_page % m_config->hbf_num_subarrays);
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
      m_active_block.subarray =
          logical_page % m_config->hbf_num_subarrays;
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
