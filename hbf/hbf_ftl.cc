// HBF Phase 2: Simple Page-Level FTL Implementation

#include "hbf_ftl.h"
#include "gpu-sim.h"
#include <algorithm>

hbf_ftl_t::hbf_ftl_t(const memory_config *config)
    : m_config(config),
      m_has_active_block(false),
      m_next_block_id(0) {
  n_translations    = 0;
  n_allocations     = 0;
  n_gcs             = 0;
  n_gc_page_copies  = 0;
  n_gc_block_erases = 0;
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
// gc() — GREEDY: pick the block with fewest valid pages, copy them out, erase
// ============================================================================
void hbf_ftl_t::gc() {
  if (!needs_gc()) return;

  // Find victim: block with fewest valid pages (GREEDY policy)
  hbf_ftl_t::block_key_t victim;
  unsigned min_valid = ~0u;
  bool found = false;

  for (auto &kv : m_blocks) {
    const hbf_ftl_t::block_key_t &bk = kv.first;
    const block_info_t &info = kv.second;

    // Skip free blocks and the active allocation block
    if (m_free_blocks.count(bk)) continue;
    if (m_has_active_block && bk.subarray == m_active_block.subarray &&
        bk.block == m_active_block.block)
      continue;
    // Only consider blocks that can be erased (have some valid pages)
    if (info.valid_pages == 0) {
      // Block with 0 valid pages: just erase it
      m_free_blocks.insert(bk);
      continue;
    }
    if (info.valid_pages < min_valid) {
      min_valid = info.valid_pages;
      victim = bk;
      found = true;
    }
  }

  if (!found) return;

  // Copy valid pages to a new block
  block_info_t &v_info = m_blocks[victim];
  n_gc_page_copies += v_info.valid_pages;

  // For simplicity: mark valid pages as needing relocation
  // In a full implementation, this would copy data between blocks
  // For now, we just track the statistics

  // Erase the victim block
  v_info.erased = true;
  v_info.free_pages = v_info.total_pages;
  v_info.next_free_page = 0;
  v_info.valid_pages = 0;
  m_free_blocks.insert(victim);

  n_gcs++;
  n_gc_block_erases++;
}

// ============================================================================
// allocate_block() — find or create a block for new writes
// ============================================================================
hbf_ftl_t::block_key_t hbf_ftl_t::allocate_block(unsigned preferred_subarray) {
  // Try free block pool first
  if (!m_free_blocks.empty()) {
    hbf_ftl_t::block_key_t bk = *m_free_blocks.begin();
    m_free_blocks.erase(m_free_blocks.begin());

    // Reset block info
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
  m_blocks[bk] = info;

  return bk;
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
  fprintf(fp, "HBF FTL Total Blocks:   %zu\n", m_blocks.size());
  fprintf(fp, "HBF FTL Free Blocks:    %zu\n", m_free_blocks.size());
}
