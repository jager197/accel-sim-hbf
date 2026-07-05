// HBF Phase 2: Simple Page-Level Flash Translation Layer
//
// Handles:
//   1. Logical page → physical (subarray, block, page) mapping
//   2. Block allocation for new writes
//   3. Garbage Collection: GREEDY victim selection
//
// Simplified vs. MQSim: no cached mapping table, no suspend/resume,
// no complex wear-leveling. Designed to demonstrate NAND behavior
// (erase-before-write, GC stalls) without MQSim's complexity.

#ifndef HBF_FTL_H
#define HBF_FTL_H

#include <map>
#include <set>
#include <stdio.h>

class memory_config;

// Physical NAND address: (subarray, block, page)
struct hbf_phys_addr_t {
  unsigned subarray;
  unsigned block;
  unsigned page;

  hbf_phys_addr_t() : subarray(0), block(0), page(0) {}
  hbf_phys_addr_t(unsigned sa, unsigned blk, unsigned pg)
      : subarray(sa), block(blk), page(pg) {}
};

class hbf_ftl_t {
 public:
  hbf_ftl_t(const memory_config *config);

  // Translate logical page → physical address.
  // On write: allocate a new physical page if needed.
  // On read:  return existing mapping (asserts it exists).
  hbf_phys_addr_t translate(unsigned long long logical_page, bool is_write);

  // Mark a logical page as no longer valid (data overwritten)
  void invalidate(unsigned long long logical_page);

  // Garbage Collection: pick victim block, copy valid pages, erase
  void gc();

  // Check if GC is needed
  bool needs_gc() const;

  // Block erase state tracking (for erase-before-write)
  bool is_block_erased(unsigned subarray, unsigned block) const;
  void mark_block_erasing(unsigned subarray, unsigned block);
  void mark_block_erased(unsigned subarray, unsigned block);

  // Statistics
  void print_stat(FILE *fp) const;

 private:
  const memory_config *m_config;

  // logical page → physical address
  std::map<unsigned long long, hbf_phys_addr_t> m_mapping;

  // Reverse: physical (subarray, block) → set of logical pages in that block
  struct block_key_t {
    unsigned subarray;
    unsigned block;
    bool operator<(const block_key_t &o) const {
      return subarray < o.subarray ||
             (subarray == o.subarray && block < o.block);
    }
  };
  struct block_info_t {
    unsigned total_pages;
    unsigned valid_pages;
    unsigned free_pages;
    unsigned next_free_page;  // next page to allocate in this block
    bool erased;
  };
  std::map<block_key_t, block_info_t> m_blocks;

  // Free block pool (ERASED, ready for allocation)
  std::set<block_key_t> m_free_blocks;

  // Active block for allocation
  block_key_t m_active_block;
  bool m_has_active_block;

  // Find or allocate a block for a new write
  block_key_t allocate_block(unsigned preferred_subarray);

  // Next block ID counter
  unsigned m_next_block_id;

  // Statistics
  unsigned long long n_translations;
  unsigned long long n_allocations;
  unsigned long long n_gcs;
  unsigned long long n_gc_page_copies;
  unsigned long long n_gc_block_erases;
};

#endif  // HBF_FTL_H
