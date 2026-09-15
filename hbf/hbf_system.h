// Homogeneous multi-stack HBF system.
//
// The GPU sees one contiguous HBF address range.  This layer selects a stack
// at page granularity, converts the request to a stack-local address while it
// is inside the device, advances all independent cubes, and restores the GPU
// address before returning the request to its source subpartition.

#ifndef HBF_SYSTEM_H
#define HBF_SYSTEM_H

#include <map>
#include <stdio.h>
#include <vector>

class gpgpu_sim;
class hbf_cube_t;
class mem_fetch;
class memory_config;
class memory_stats_t;

struct hbf_stack_location_t {
  hbf_stack_location_t()
      : stack(0), global_page(0), local_page(0), page_offset(0),
        local_offset(0) {}

  unsigned stack;
  unsigned long long global_page;
  unsigned long long local_page;
  unsigned long long page_offset;
  unsigned long long local_offset;
};

class hbf_system_t {
 public:
  hbf_system_t(const memory_config *config, memory_stats_t *stats,
               gpgpu_sim *gpu);
  ~hbf_system_t();

  bool full(unsigned long long address, bool is_write) const;
  void push(mem_fetch *request);
  void cycle();
  mem_fetch *pop_return_for(unsigned global_subpartition_id);

  bool busy() const;
  unsigned num_stacks() const { return m_num_stacks; }
  unsigned channels_per_stack() const;
  void print_stat(FILE *fp);

  // Pure mapping helper used by focused tests and experiment tooling.  Keep
  // this header-only so the address policy can be validated without linking
  // the simulator or constructing mem_fetch objects.
  static unsigned long long logical_address_for_mapping(
      unsigned long long address, unsigned long long base_address,
      bool route_all) {
    // Route-all models the complete GPU address space, so the absolute address
    // is the logical identity.  Range mode retains the legacy base-relative
    // convention used by one-stack configurations.
    return route_all || address < base_address ? address
                                               : address - base_address;
  }

  static bool capacity_has_equal_whole_blocks(
      unsigned long long total_capacity, unsigned page_size,
      unsigned pages_per_block, unsigned num_stacks) {
    if (total_capacity == 0 || page_size == 0 || pages_per_block == 0 ||
        num_stacks == 0 || total_capacity % page_size != 0)
      return false;
    const unsigned long long total_pages = total_capacity / page_size;
    return total_pages % num_stacks == 0 &&
           (total_pages / num_stacks) % pages_per_block == 0;
  }

  static hbf_stack_location_t map_relative_address(
      unsigned long long relative_address,
      unsigned long long total_capacity, unsigned page_size,
      unsigned num_stacks, int stack_map) {
    hbf_stack_location_t location;
    if (page_size == 0 || num_stacks == 0) return location;

    location.global_page = relative_address / page_size;
    location.page_offset = relative_address % page_size;
    const unsigned long long total_pages = total_capacity / page_size;
    const unsigned long long pages_per_stack = total_pages / num_stacks;

    if (stack_map == 1 && pages_per_stack > 0 &&
        location.global_page < total_pages) {
      location.stack =
          (unsigned)(location.global_page / pages_per_stack);
      if (location.stack >= num_stacks) location.stack = num_stacks - 1;
      location.local_page =
          location.global_page - location.stack * pages_per_stack;
    } else {
      // Page interleaving is also the deterministic route-all fallback for
      // addresses outside the configured span.  It preserves uniqueness and
      // avoids pinning every such request to the final contiguous stack.
      location.stack = (unsigned)(location.global_page % num_stacks);
      location.local_page = location.global_page / num_stacks;
    }
    location.local_offset =
        location.local_page * page_size + location.page_offset;
    return location;
  }

 private:
  hbf_stack_location_t locate(unsigned long long address) const;
  void restore_address(mem_fetch *request);

  const memory_config *m_config;
  std::vector<hbf_cube_t *> m_stacks;
  std::vector<unsigned long long> m_routed_requests;
  std::vector<unsigned long long> m_completed_requests;
  std::map<unsigned, unsigned> m_return_rr_next;
  std::map<mem_fetch *, unsigned long long> m_original_addresses;
  unsigned m_num_stacks;
  int m_stack_map;
  bool m_rewrite_addresses;
  unsigned long long m_total_capacity;
  unsigned long long m_stack_capacity;
};

#endif  // HBF_SYSTEM_H
