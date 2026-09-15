#include <cassert>
#include <set>
#include <tuple>

#include "hbf_system.h"

namespace {

const unsigned kPageSize = 4096;

void test_single_stack_is_identity() {
  const unsigned long long capacity = 64ull * kPageSize;
  for (unsigned long long page = 0; page < 64; ++page) {
    const unsigned offset = (unsigned)((page * 37) % kPageSize);
    hbf_stack_location_t location = hbf_system_t::map_relative_address(
        page * kPageSize + offset, capacity, kPageSize, 1, 0);
    assert(location.stack == 0);
    assert(location.global_page == page);
    assert(location.local_page == page);
    assert(location.page_offset == offset);
    assert(location.local_offset == page * kPageSize + offset);
  }
}

void test_page_interleave_is_balanced_and_reversible() {
  const unsigned stacks = 4;
  const unsigned long long pages = 64;
  const unsigned long long capacity = pages * kPageSize;
  unsigned counts[stacks] = {};
  std::set<std::tuple<unsigned, unsigned long long> > identities;

  for (unsigned long long page = 0; page < pages; ++page) {
    hbf_stack_location_t location = hbf_system_t::map_relative_address(
        page * kPageSize + 123, capacity, kPageSize, stacks, 0);
    assert(location.stack == page % stacks);
    assert(location.local_page == page / stacks);
    assert(location.page_offset == 123);
    assert(location.local_offset == location.local_page * kPageSize + 123);
    assert(location.local_page * stacks + location.stack == page);
    assert(identities.insert(
                         std::make_tuple(location.stack, location.local_page))
               .second);
    counts[location.stack]++;
  }

  for (unsigned stack = 0; stack < stacks; ++stack)
    assert(counts[stack] == pages / stacks);
}

void test_contiguous_map_uses_equal_capacity_slices() {
  const unsigned stacks = 4;
  const unsigned long long pages_per_stack = 16;
  const unsigned long long capacity =
      stacks * pages_per_stack * kPageSize;

  for (unsigned long long page = 0; page < stacks * pages_per_stack; ++page) {
    hbf_stack_location_t location = hbf_system_t::map_relative_address(
        page * kPageSize + 4095, capacity, kPageSize, stacks, 1);
    const unsigned expected_stack = (unsigned)(page / pages_per_stack);
    const unsigned long long expected_local = page % pages_per_stack;
    assert(location.stack == expected_stack);
    assert(location.local_page == expected_local);
    assert(location.page_offset == 4095);
    assert(location.local_offset == expected_local * kPageSize + 4095);
    assert(location.stack * pages_per_stack + location.local_page == page);
  }
}

void test_route_all_fallback_remains_deterministic() {
  // Contiguous mapping has no finite last slice for route-all addresses
  // beyond the configured HBF span. The helper intentionally falls back to
  // page interleaving rather than aliasing them onto the final stack.
  const unsigned stacks = 2;
  const unsigned long long capacity = 8ull * kPageSize;
  const unsigned long long page = 11;
  hbf_stack_location_t location = hbf_system_t::map_relative_address(
      page * kPageSize + 17, capacity, kPageSize, stacks, 1);
  assert(location.stack == page % stacks);
  assert(location.local_page == page / stacks);
  assert(location.local_offset == location.local_page * kPageSize + 17);
}

void test_route_all_uses_absolute_gpu_address_identity() {
  const unsigned long long base = 256ull * 1024 * 1024 * 1024;
  const unsigned long long low_address = 3ull * kPageSize + 29;
  const unsigned long long ranged_address = base + low_address;
  const unsigned long long capacity = 64ull * kPageSize;

  const unsigned long long low_logical =
      hbf_system_t::logical_address_for_mapping(low_address, base, true);
  const unsigned long long ranged_logical =
      hbf_system_t::logical_address_for_mapping(ranged_address, base, true);
  assert(low_logical == low_address);
  assert(ranged_logical == ranged_address);
  assert(low_logical != ranged_logical);

  hbf_stack_location_t low = hbf_system_t::map_relative_address(
      low_logical, capacity, kPageSize, 1, 0);
  hbf_stack_location_t ranged = hbf_system_t::map_relative_address(
      ranged_logical, capacity, kPageSize, 1, 0);
  assert(low.local_offset != ranged.local_offset);

  // Normal range mode keeps its historical base-relative behavior.
  assert(hbf_system_t::logical_address_for_mapping(ranged_address, base, false) ==
         low_address);
}

void test_capacity_is_equal_and_block_aligned_per_stack() {
  const unsigned stacks = 4;
  const unsigned pages_per_block = 16;
  const unsigned long long valid_capacity =
      (unsigned long long)stacks * pages_per_block * 3 * kPageSize;
  assert(hbf_system_t::capacity_has_equal_whole_blocks(
      valid_capacity, kPageSize, pages_per_block, stacks));
  assert(!hbf_system_t::capacity_has_equal_whole_blocks(
      valid_capacity + kPageSize, kPageSize, pages_per_block, stacks));
  assert(!hbf_system_t::capacity_has_equal_whole_blocks(
      valid_capacity - stacks * kPageSize, kPageSize, pages_per_block,
      stacks));
  assert(!hbf_system_t::capacity_has_equal_whole_blocks(
      valid_capacity, 0, pages_per_block, stacks));
}

}  // namespace

int main() {
  test_single_stack_is_identity();
  test_page_interleave_is_balanced_and_reversible();
  test_contiguous_map_uses_equal_capacity_slices();
  test_route_all_fallback_remains_deterministic();
  test_route_all_uses_absolute_gpu_address_identity();
  test_capacity_is_equal_and_block_aligned_per_stack();
  return 0;
}
