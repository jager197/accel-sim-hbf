#include <cassert>
#include <set>
#include <utility>

#include "hbf_controller.h"

int main() {
  hbf_write_coverage_t coverage(4096);
  unsigned added = 0;

  // Repeated writes to the same cache line do not create false coverage.
  for (unsigned i = 0; i < 64; ++i) {
    assert(coverage.add(0, 64, &added));
    assert(added == (i == 0 ? 64u : 0u));
  }
  assert(coverage.covered_bytes() == 64);
  assert(!coverage.full());

  // Arbitrary non-aligned, overlapping byte ranges are counted exactly.
  coverage.reset(4096);
  assert(coverage.add(13, 100, &added));
  assert(added == 100);
  assert(coverage.add(63, 80, &added));
  assert(added == 30);
  assert(coverage.covered_bytes() == 130);

  // Sixty-four distinct 64-byte writes cover one 4 KiB page exactly.
  coverage.reset(4096);
  for (unsigned offset = 0; offset < 4096; offset += 64) {
    assert(coverage.add(offset, 64, &added));
    assert(added == 64);
  }
  assert(coverage.covered_bytes() == 4096);
  assert(coverage.full());

  // A malformed cross-page request must not partially mutate accounting.
  assert(!coverage.add(4090, 16, &added));
  assert(added == 0);
  assert(coverage.covered_bytes() == 4096);

  // Sector requests can share an address and differ only in their byte mask.
  // The two masks together cover 64 bytes, not one duplicated 32-byte range.
  mem_access_byte_mask_t first_sector;
  mem_access_byte_mask_t second_sector;
  for (unsigned i = 0; i < 32; ++i) first_sector.set(i);
  for (unsigned i = 32; i < 64; ++i) second_sector.set(i);
  coverage.reset(4096);
  assert(coverage.add_mask(0, first_sector, &added));
  assert(added == 32);
  assert(coverage.add_mask(0, second_sector, &added));
  assert(added == 32);
  assert(coverage.covered_bytes() == 64);

  // Mask validation is atomic: an out-of-page bit rejects the whole mask.
  mem_access_byte_mask_t crossing;
  crossing.set(0);
  crossing.set(127);
  assert(!coverage.add_mask(4000, crossing, &added));
  assert(added == 0);
  assert(coverage.covered_bytes() == 64);

  // An empty byte mask uses the production fallback: one contiguous range.
  mem_access_byte_mask_t empty;
  assert(empty.none());
  assert(coverage.add(128, 32, &added));
  assert(added == 32);
  assert(coverage.covered_bytes() == 96);

  // Non-FTL pages have a stable, collision-free direct block/page identity.
  std::set<std::pair<unsigned, unsigned>> identities;
  for (unsigned long long page = 0; page < 1024; ++page) {
    unsigned block = 0, page_offset = 0;
    assert(hbf_direct_page_identity(page, 4, &block, &page_offset));
    assert((block & (1u << 31)) != 0);
    assert(identities.insert(std::make_pair(block, page_offset)).second);
    unsigned repeat_block = 0, repeat_offset = 0;
    assert(hbf_direct_page_identity(page, 4, &repeat_block, &repeat_offset));
    assert(repeat_block == block && repeat_offset == page_offset);
  }

  // Coalescing more requests than the return queue can hold still makes
  // progress in bounded batches instead of waiting for impossible capacity.
  assert(hbf_delivery_batch_limit(9, 0, 4) == 4);
  assert(hbf_delivery_batch_limit(5, 3, 4) == 1);
  assert(hbf_delivery_batch_limit(2, 4, 4) == 0);

  std::vector<hbf_schedule_candidate_t> candidates = {
      {true, false, false, false},  // queue head targets a busy subarray
      {true, false, true, false},
      {true, false, true, true},
  };
  assert(hbf_pick_schedule_candidate(candidates, false, false) == 1);
  assert(hbf_pick_schedule_candidate(candidates, true, true) == 2);
  assert(hbf_count_deferred_reads(candidates, 1, true) == 1);
  assert(hbf_count_deferred_reads(candidates, 2, true) == 0);
  assert(hbf_count_deferred_reads(candidates, 1, false) == 0);
  candidates[2].aggregation_pending = true;
  assert(hbf_pick_schedule_candidate(candidates, true, true) == 1);
  candidates[1].subarray_idle = false;
  assert(hbf_pick_schedule_candidate(candidates, true, true) == -1);

  // An expired drain window requires a read service opportunity before it can
  // reopen under the same sustained write pressure.
  unsigned long long drain_until = 0;
  bool read_due = false;
  assert(hbf_update_write_drain(4, 1, 10, 5, &drain_until, &read_due) ==
         HBF_DRAIN_OPENED);
  assert(drain_until == 15 && !read_due);
  assert(hbf_update_write_drain(4, 1, 14, 5, &drain_until, &read_due) ==
         HBF_DRAIN_UNCHANGED);
  assert(hbf_update_write_drain(4, 1, 15, 5, &drain_until, &read_due) ==
         HBF_DRAIN_EXPIRED);
  assert(drain_until == 0 && read_due);
  assert(hbf_update_write_drain(4, 1, 16, 5, &drain_until, &read_due) ==
         HBF_DRAIN_UNCHANGED);
  read_due = false;
  assert(hbf_update_write_drain(4, 1, 17, 5, &drain_until, &read_due) ==
         HBF_DRAIN_OPENED);
  assert(hbf_update_write_drain(0, 1, 18, 5, &drain_until, &read_due) ==
         HBF_DRAIN_RELEASED);
  assert(drain_until == 0 && !read_due);

  assert(hbf_write_padding_bytes(4096, 4096) == 0);
  assert(hbf_write_padding_bytes(4096, 96) == 4000);
  assert(hbf_effective_write_bytes(first_sector, 64) == 32);
  assert(hbf_effective_write_bytes(empty, 64) == 64);

  // Link credits cannot accumulate hundreds of idle ticks into an oversized
  // same-tick burst. Slow links retain one maximum transaction for progress.
  hbf_channel_t slow_channel(0, 0, 1, 1.0);
  for (unsigned tick = 0; tick < 127; ++tick) slow_channel.replenish_credit();
  assert(!slow_channel.try_consume_credit(128));
  slow_channel.replenish_credit();
  assert(slow_channel.try_consume_credit(128));
  assert(!slow_channel.try_consume_credit(1));

  hbf_channel_t fast_channel(0, 0, 1, 256.0);
  fast_channel.replenish_credit();
  assert(fast_channel.try_consume_credit(128));
  assert(fast_channel.try_consume_credit(128));
  assert(!fast_channel.try_consume_credit(1));
  return 0;
}
