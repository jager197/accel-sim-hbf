// HBF v0.4: Shared Page Cache (SRAM on logic die)
//
// An LRU cache of recently-read NAND pages, sitting between the MSHR
// layer and sub-array scheduling. Cache hits avoid the full NAND page
// read latency (tR ≈ 15K cycles), returning data in cache_hit_latency
// (~50 cycles).
//
// Write policy: write-invalidate — a program or erase to a page removes
// it from the cache, since the old data is stale.
//
// Indexed by physical page address (after FTL translation), so FTL
// remapping does not cause coherency issues.

#ifndef HBF_PAGE_CACHE_H
#define HBF_PAGE_CACHE_H

#include <list>
#include <map>

class hbf_page_cache_t {
 public:
  // max_entries = 0 disables caching (all lookups miss)
  hbf_page_cache_t(unsigned max_entries);

  // Look up a physical page address. Returns true on hit.
  // On hit, promotes the page to MRU position.
  bool lookup(unsigned long long phys_page);

  // Insert a page into the cache. Evicts LRU entry if full.
  void insert(unsigned long long phys_page);

  // Invalidate a page (e.g., on program or erase).
  void invalidate(unsigned long long phys_page);

  // Cache size
  unsigned size() const { return (unsigned)m_lru_list.size(); }
  bool enabled() const { return m_max_entries > 0; }

  // Statistics (public for easy aggregation in controller)
  unsigned long long hits;
  unsigned long long misses;
  unsigned long long evictions;

 private:
  unsigned m_max_entries;

  // LRU list: front = most-recently-used, back = least-recently-used
  std::list<unsigned long long> m_lru_list;

  // page_addr → position in m_lru_list (for O(1) promotion / removal)
  std::map<unsigned long long,
           std::list<unsigned long long>::iterator> m_index;
};

#endif  // HBF_PAGE_CACHE_H
