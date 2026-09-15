// HBF Phase 2: Simple Page-Level FTL Implementation

#include "hbf_ftl.h"
#include "gpu-sim.h"
#include "hbf_channel.h"
#include "hbf_mapping.h"
#include <algorithm>

namespace {
const unsigned kPseudoBlockBit = 1u << 31;

unsigned hbf_ftl_num_channels(const memory_config *cfg) {
  unsigned nch = cfg->hbf_num_channels;
  if (nch == 0) nch = 1;
  if (nch > 16 && !cfg->hbf_allow_non_ocp_channels) nch = 16;
  unsigned nsa = cfg->hbf_num_subarrays > 0 ? cfg->hbf_num_subarrays : 1;
  if (nch > nsa) nch = nsa;
  return nch;
}

// Channel-aware preferred sub-array for a logical page (v0.4): allocation
// must land inside the page's own host channel (OCP §4.5 — a channel can
// only use its own die set). Also returns a channel-local page ordinal so
// interleaved pages advance through every sub-array owned by that channel.
bool hbf_ftl_page_location(const memory_config *cfg, unsigned long long page,
                           const std::map<unsigned long long, unsigned>
                               *explicit_map,
                           unsigned *channel,
                           unsigned long long *channel_local_page) {
  const unsigned nch = hbf_ftl_num_channels(cfg);
  if (cfg->hbf_placement_mode == 2) {
    if (explicit_map == NULL) return false;
    auto mapped = explicit_map->find(page);
    if (mapped == explicit_map->end() || mapped->second >= nch) return false;
    *channel = mapped->second;
    // Explicit placement defines the channel, while the logical page number
    // supplies a deterministic spread within that channel.
    *channel_local_page = page;
    return true;
  }

  unsigned long long total_pages =
      cfg->hbf_page_size ? cfg->hbf_size / cfg->hbf_page_size : 0;
  unsigned long long pages_per_ch =
      (total_pages + nch - 1) / nch;
  if (pages_per_ch == 0) pages_per_ch = 1;
  bool contiguous = cfg->hbf_placement_mode == 1 ||
                    (cfg->hbf_placement_mode == 0 &&
                     cfg->hbf_channel_map == 1);
  if (contiguous) {
    unsigned ch = (unsigned)(page / pages_per_ch);
    if (ch >= nch) ch = nch - 1;
    *channel = ch;
    *channel_local_page = page - (unsigned long long)ch * pages_per_ch;
  } else {
    *channel = nch <= 1 ? 0 : (unsigned)(page % nch);
    *channel_local_page = page / nch;
  }
  return true;
}

// Preferred (sub-array, channel-local spread) for a logical page: a stable
// sub-array inside the page's channel, so channel interleaving spreads
// pages across the channel's die set.
bool hbf_ftl_preferred_subarray(const memory_config *cfg,
                                unsigned long long page,
                                const std::map<unsigned long long, unsigned>
                                    *explicit_map,
                                unsigned *subarray) {
  const unsigned nch = hbf_ftl_num_channels(cfg);
  unsigned nsa = cfg->hbf_num_subarrays > 0 ? cfg->hbf_num_subarrays : 1;
  unsigned per_ch = nsa / nch;
  if (per_ch == 0) per_ch = 1;
  unsigned ch = 0;
  unsigned long long channel_local_page = 0;
  if (!hbf_ftl_page_location(cfg, page, explicit_map, &ch,
                             &channel_local_page))
    return false;
  unsigned first = ch * per_ch;
  unsigned count = ch == nch - 1 ? nsa - first : per_ch;
  if (count == 0) count = 1;
  *subarray = first + (unsigned)(channel_local_page % count);
  return true;
}

// Never-written reads must participate in timing without consuming FTL
// capacity. Reserve the high block-id bit for a stable direct-map namespace,
// keeping every logical page distinct from allocated physical pages.
hbf_phys_addr_t hbf_ftl_pseudo_address(const memory_config *cfg,
                                       unsigned long long logical_page,
                                       const std::map<unsigned long long,
                                                      unsigned> *explicit_map) {
  unsigned pages_per_block = cfg->hbf_pages_per_block;
  if (pages_per_block == 0) pages_per_block = 1;
  unsigned long long pseudo_block = logical_page / pages_per_block;
  if (pseudo_block >= kPseudoBlockBit) {
    return hbf_phys_addr_t(~0u, ~0u, ~0u);
  }
  unsigned subarray = 0;
  if (!hbf_ftl_preferred_subarray(cfg, logical_page, explicit_map, &subarray))
    return hbf_phys_addr_t(~0u, ~0u, ~0u);
  return hbf_phys_addr_t(subarray, kPseudoBlockBit | (unsigned)pseudo_block,
                         (unsigned)(logical_page % pages_per_block));
}
}  // namespace

hbf_ftl_t::hbf_ftl_t(const memory_config *config)
    : m_config(config),
      m_gc_active(false),
      m_gc_cycles_remaining(0),
      m_gc_victim_subarray(0),
      m_next_block_id(0),
      m_max_blocks(0),
      n_capacity_exceeded(0),
      m_capacity_error(false),
      m_mapping_error(false),
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

  if (m_config->hbf_placement_mode == 2) {
    std::string error;
    if (!hbf_load_placement_table(m_config->hbf_channel_map_file,
                                  hbf_ftl_num_channels(m_config),
                                  &m_explicit_channel_map, &error)) {
      fprintf(stderr, "HBF FTL fatal: %s\n", error.c_str());
      fflush(stderr);
      exit(EXIT_FAILURE);
    }
  }

  // ── Capacity enforcement (v0.4) ─────────────────────────────────────
  // The configured HBF capacity belongs to the logical cube.  The cube owns
  // one FTL, so capacity is not divided or multiplied by GPU memory
  // partitions.
  unsigned long long bytes_per_cube = m_config->hbf_size;
  unsigned long long bytes_per_block =
      (unsigned long long)m_config->hbf_pages_per_block *
      m_config->hbf_page_size;
  m_max_blocks = bytes_per_block ? bytes_per_cube / bytes_per_block : 0;
  if (m_max_blocks == 0 && bytes_per_cube != 0) m_max_blocks = 1;
}

// ============================================================================
// translate() — logical page → physical address
// ============================================================================
hbf_phys_addr_t hbf_ftl_t::translate(unsigned long long logical_page,
                                     bool is_write) {
  n_translations++;

  auto old_mapping = m_mapping.find(logical_page);
  if (old_mapping != m_mapping.end() && !is_write) return old_mapping->second;

  if (old_mapping == m_mapping.end() && !is_write) {
    // Reads of never-written pages do NOT allocate (FTL semantics — a read
    // never creates a mapping). Direct-map to a stable pseudo-physical
    // location so the timing model still has a sub-array to schedule on.
    // The pseudo location lies inside the page's own channel (v0.4).
    // Without this, read-only streaming workloads fill every block and
    // trigger GC on every new block (O(n^2) churn / multi-hour hangs).
    hbf_phys_addr_t pseudo = hbf_ftl_pseudo_address(
        m_config, logical_page, &m_explicit_channel_map);
    if (!pseudo.valid()) m_mapping_error = true;
    return pseudo;
  }

  unsigned preferred_subarray = 0;
  if (!hbf_ftl_preferred_subarray(m_config, logical_page,
                                  &m_explicit_channel_map,
                                  &preferred_subarray)) {
    m_mapping_error = true;
    return hbf_phys_addr_t(~0u, ~0u, ~0u);
  }
  auto active_it = m_active_blocks.find(preferred_subarray);
  if (active_it == m_active_blocks.end()) {
    block_key_t block = allocate_block(preferred_subarray);
    if (block.subarray == ~0u) {
      return hbf_phys_addr_t(~0u, ~0u, ~0u);
    }
    active_it = m_active_blocks.insert(
        std::make_pair(preferred_subarray, block)).first;
  }

  const hbf_ftl_t::block_key_t bk = active_it->second;
  auto blk_it = m_blocks.find(bk);
  assert(blk_it != m_blocks.end());
  assert(blk_it->second.free_pages > 0);

  unsigned page = blk_it->second.next_free_page;
  blk_it->second.next_free_page++;
  blk_it->second.free_pages--;
  blk_it->second.valid_pages++;

  // If this block is full, need a new one next time
  if (blk_it->second.free_pages == 0) {
    m_active_blocks.erase(preferred_subarray);
  }

  hbf_phys_addr_t phys(bk.subarray, bk.block, page);

  // Commit overwrite only after the replacement page has been reserved.
  // If allocation fails above, the previous mapping and reverse index remain
  // untouched and reads continue to observe the old physical page.
  if (old_mapping != m_mapping.end()) invalidate(logical_page);
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
      // A block containing invalid programmed pages must be erased before it
      // can be reused. Retire it from the per-subarray allocation cursor.
      blk_it->second.erased = false;
      deactivate_block(bk);
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

bool hbf_ftl_t::is_active_block(const block_key_t &block) const {
  auto it = m_active_blocks.find(block.subarray);
  return it != m_active_blocks.end() &&
         it->second.block == block.block;
}

void hbf_ftl_t::deactivate_block(const block_key_t &block) {
  auto it = m_active_blocks.find(block.subarray);
  if (it != m_active_blocks.end() && it->second.block == block.block) {
    m_active_blocks.erase(it);
  }
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
// physical pages, fixing the stale-mapping bug. Models relocation latency as
// valid_pages × tPROG cycles and returns the victim in an erased state.
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

    // Skip free blocks and every subarray's active allocation block.
    if (m_free_blocks.count(bk)) continue;
    if (is_active_block(bk)) continue;
    // A zero-valid block can enter the free pool as dirty; the controller
    // performs ERASE if it is selected for a later PROGRAM.
    if (info.valid_pages == 0) {
      m_blocks[bk].erased = false;
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
      if (!new_phys.valid()) {
        m_capacity_error = true;
        return;
      }

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
  deactivate_block(victim);
  m_free_blocks.insert(victim);

  // ── Step 3: Model GC latency ──
  // Each relocated page costs one tPROG cycle on the target subarray.
  // The simplified SSD-GC state transition above accounts the victim as
  // erased; it must not incur a second controller-side ERASE on reuse.
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
  unsigned preferred_subarray = 0;
  if (!hbf_ftl_preferred_subarray(m_config, logical_page,
                                  &m_explicit_channel_map,
                                  &preferred_subarray)) {
    m_mapping_error = true;
    return hbf_phys_addr_t(~0u, ~0u, ~0u);
  }
  auto active_it = m_active_blocks.find(preferred_subarray);
  if (active_it == m_active_blocks.end()) {
    // GC relocation cannot recursively invoke gc(). Reuse only a block from
    // the required subarray; taking a block from another subarray would break
    // the page's channel affinity.
    block_key_t block = {~0u, ~0u};
    for (auto free_it = m_free_blocks.begin();
         free_it != m_free_blocks.end(); ++free_it) {
      if (free_it->subarray == preferred_subarray) {
        block = *free_it;
        m_free_blocks.erase(free_it);
        break;
      }
    }
    if (block.subarray != ~0u) {
      block_info_t &info = m_blocks[block];
      info.free_pages = info.total_pages;
      info.next_free_page = 0;
      info.valid_pages = 0;
      // Preserve the free-pool erase state: HBF invalidation contributes a
      // dirty block, while SSD GC contributes an already-erased block.
    } else {
      if (m_blocks.size() >= m_max_blocks ||
          m_next_block_id >= kPseudoBlockBit) {
        n_capacity_exceeded++;
        m_capacity_error = true;
        return hbf_phys_addr_t(~0u, ~0u, ~0u);
      }
      block.subarray = preferred_subarray;
      block.block = m_next_block_id++;

      block_info_t info;
      info.total_pages = m_config->hbf_pages_per_block;
      info.free_pages = m_config->hbf_pages_per_block;
      info.valid_pages = 0;
      info.next_free_page = 0;
      info.erased = true;
      info.erase_count = 0;
      m_blocks[block] = info;
    }
    active_it = m_active_blocks.insert(
        std::make_pair(preferred_subarray, block)).first;
  }

  const hbf_ftl_t::block_key_t bk = active_it->second;
  auto blk_it = m_blocks.find(bk);
  assert(blk_it != m_blocks.end());
  assert(blk_it->second.free_pages > 0);

  unsigned page = blk_it->second.next_free_page;
  blk_it->second.next_free_page++;
  blk_it->second.free_pages--;
  blk_it->second.valid_pages++;

  if (blk_it->second.free_pages == 0) {
    m_active_blocks.erase(preferred_subarray);
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
  // Try only free blocks in the requested subarray. Cross-subarray reuse
  // would make the FTL mapping disagree with channel ownership.
  block_key_t free_block = {~0u, ~0u};
  unsigned long long min_erase = ~0ull;
  for (const block_key_t &candidate : m_free_blocks) {
    if (candidate.subarray != preferred_subarray) continue;
    auto block_it = m_blocks.find(candidate);
    if (block_it == m_blocks.end()) continue;
    if (!m_config->hbf_wear_leveling_enabled) {
      free_block = candidate;
      break;
    }
    if (block_it->second.erase_count < min_erase) {
      min_erase = block_it->second.erase_count;
      free_block = candidate;
    }
  }
  if (free_block.subarray != ~0u) {
    m_free_blocks.erase(free_block);
    block_info_t &info = m_blocks[free_block];
    info.free_pages = info.total_pages;
    info.next_free_page = 0;
    info.valid_pages = 0;
    // Preserve whether the producer of this free block already erased it.
    if (m_config->hbf_wear_leveling_enabled) n_wl_biased_allocs++;
    return free_block;
  }

  // Check if GC can free up blocks
  if (needs_gc()) {
    gc();
    for (const block_key_t &candidate : m_free_blocks) {
      if (candidate.subarray == preferred_subarray) {
      return allocate_block(preferred_subarray);
      }
    }
  }

  // ── Capacity enforcement ────────────────────────────────────────────
  // The device is bounded: never create a block beyond m_max_blocks. A
  // capacity error is propagated to the controller instead of recycling a
  // live block and silently invalidating its logical mappings.
  if (m_blocks.size() >= m_max_blocks ||
      m_next_block_id >= kPseudoBlockBit) {
    n_capacity_exceeded++;
    m_capacity_error = true;
    return block_key_t{~0u, ~0u};
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
  info.erased         = true;
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
  auto it = m_blocks.find(bk);
  assert(it != m_blocks.end());
  it->second.erased = false;  // block is being erased, not yet ready
}

void hbf_ftl_t::mark_block_erased(unsigned subarray, unsigned block) {
  hbf_ftl_t::block_key_t bk;
  bk.subarray = subarray;
  bk.block    = block;
  auto it = m_blocks.find(bk);
  assert(it != m_blocks.end());
  // Allocation reserves page offsets before the controller schedules ERASE.
  // Do not reset next_free_page/free_pages here or later reservations would
  // reuse page zero. The block was empty when selected from the free pool.
  it->second.erased = true;
  it->second.erase_count++;  // physical erase completed -> increment wear
}

// ============================================================================
// get_usage() — compute capacity usage summary
// ============================================================================
hbf_usage_info_t hbf_ftl_t::get_usage() const {
  hbf_usage_info_t u;
  u.configured_pages = m_config->hbf_page_size
                           ? m_config->hbf_size / m_config->hbf_page_size
                           : 0;
  u.allocated_pages = 0;
  u.total_pages = 0;
  u.valid_pages = 0;
  u.free_pages = 0;
  u.total_blocks = m_blocks.size();
  u.free_blocks = m_free_blocks.size();
  u.logical_pages = m_mapping.size();
  u.subarrays_used = 0;
  u.subarrays_total =
      m_config->hbf_num_subarrays > 0 ? m_config->hbf_num_subarrays : 1;
  for (unsigned i = 0; i < 5; i++) u.histogram[i] = 0;

  // Per-subarray accumulation: subarray → (valid pages, total pages)
  std::map<unsigned, std::pair<unsigned long long, unsigned long long>> per_sa;

  for (auto &kv : m_blocks) {
    const block_info_t &info = kv.second;
    u.total_pages += info.total_pages;
    u.allocated_pages += info.total_pages;
    u.valid_pages += info.valid_pages;
    u.free_pages += info.free_pages;
    per_sa[kv.first.subarray].first += info.valid_pages;
    per_sa[kv.first.subarray].second += info.total_pages;
  }

  for (unsigned sa = 0; sa < u.subarrays_total; ++sa) {
    auto found = per_sa.find(sa);
    unsigned long long valid =
        found == per_sa.end() ? 0 : found->second.first;
    unsigned long long total =
        found == per_sa.end() ? 0 : found->second.second;
    if (valid > 0) ++u.subarrays_used;
    double frac = total > 0
                      ? (double)valid / total
                      : 0.0;
    int bucket = (valid == 0)             ? 0
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
  fprintf(fp, "HBF FTL Capacity:       %llu / %llu max blocks (%.2f GiB modeled per cube)\n",
          (unsigned long long)m_blocks.size(), m_max_blocks,
          (double)m_max_blocks * m_config->hbf_pages_per_block *
              m_config->hbf_page_size / (1024.0 * 1024 * 1024));
  fprintf(fp, "HBF FTL Cap Exceeded:   %llu\n", n_capacity_exceeded);
  fprintf(fp, "HBF FTL Capacity Error:  %s\n",
          m_capacity_error ? "yes" : "no");
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
  double util = u.configured_pages > 0
                    ? 100.0 * u.valid_pages / u.configured_pages
                    : 0.0;
  fprintf(fp, "HBF Capacity Usage:    %llu live / %llu configured pages (%.2f%%)\n",
          u.valid_pages, u.configured_pages, util);
  fprintf(fp, "HBF Capacity Bytes:    %llu live / %llu configured bytes\n",
          u.valid_pages * m_config->hbf_page_size,
          u.configured_pages * m_config->hbf_page_size);
  fprintf(fp, "HBF Allocated Metadata: %llu pages (%llu bytes)\n",
          u.allocated_pages,
          u.allocated_pages * m_config->hbf_page_size);
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
