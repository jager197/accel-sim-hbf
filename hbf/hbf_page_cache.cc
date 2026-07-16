// HBF v0.4: Shared Page Cache Implementation

#include "hbf_page_cache.h"

hbf_page_cache_t::hbf_page_cache_t(unsigned max_entries)
    : m_max_entries(max_entries) {
  hits      = 0;
  misses    = 0;
  evictions = 0;
}

bool hbf_page_cache_t::lookup(unsigned long long phys_page) {
  if (m_max_entries == 0) {
    misses++;
    return false;
  }

  auto it = m_index.find(phys_page);
  if (it != m_index.end()) {
    // Hit: move to front of LRU list (MRU position)
    m_lru_list.erase(it->second);
    m_lru_list.push_front(phys_page);
    it->second = m_lru_list.begin();
    hits++;
    return true;
  }

  misses++;
  return false;
}

void hbf_page_cache_t::insert(unsigned long long phys_page) {
  if (m_max_entries == 0) return;

  // Already in cache? Just update LRU position (should not happen in
  // normal flow, but handle gracefully)
  auto it = m_index.find(phys_page);
  if (it != m_index.end()) {
    m_lru_list.erase(it->second);
    m_lru_list.push_front(phys_page);
    it->second = m_lru_list.begin();
    return;
  }

  // Evict LRU if full
  if (m_lru_list.size() >= m_max_entries) {
    unsigned long long victim = m_lru_list.back();
    m_lru_list.pop_back();
    m_index.erase(victim);
    evictions++;
  }

  // Insert at front (MRU)
  m_lru_list.push_front(phys_page);
  m_index[phys_page] = m_lru_list.begin();
}

void hbf_page_cache_t::invalidate(unsigned long long phys_page) {
  auto it = m_index.find(phys_page);
  if (it != m_index.end()) {
    m_lru_list.erase(it->second);
    m_index.erase(it);
  }
}
