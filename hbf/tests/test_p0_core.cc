#include <cassert>
#include <cstdio>
#include <cstring>
#include <set>
#include <string>
#include <tuple>
#include <unistd.h>

#include "gpu-sim.h"
#include "hbf_ftl.h"
#include "hbf_mapping.h"
#include "hbf_subarray.h"

// hbf_ftl_t only reads scalar HBF fields from memory_config.  These minimal
// definitions satisfy unrelated objects constructed as part of memory_config
// without pulling the full simulator into this focused component test.
linear_to_raw_address_translation::linear_to_raw_address_translation() {}
unsigned cache_config::set_index(new_addr_type) const { return 0; }
unsigned l2_cache_config::set_index(new_addr_type) const { return 0; }

namespace {

memory_config make_config() {
  memory_config cfg(nullptr);
  cfg.hbf_num_channels = 4;
  cfg.hbf_allow_non_ocp_channels = false;
  cfg.hbf_num_subarrays = 8;
  cfg.hbf_page_size = 4096;
  cfg.hbf_pages_per_block = 4;
  cfg.hbf_size = 4096ull * 4 * 64;
  cfg.hbf_media_mode = 0;
  cfg.hbf_blocks_per_zone = 64;
  cfg.hbf_zone_remap_enabled = false;
  cfg.hbf_zone_remap_threshold = 100;
  cfg.hbf_placement_mode = 0;
  cfg.hbf_channel_map = 0;
  cfg.hbf_channel_map_file = const_cast<char *>("");
  cfg.hbf_wear_leveling_enabled = false;
  cfg.hbf_overprovisioning = 0.1f;
  cfg.hbf_tR = 3;
  cfg.hbf_tPROG = 5;
  cfg.hbf_tBERS = 7;
  cfg.hbf_buffer_hit_latency = 1;
  cfg.hbf_buffer_enabled = true;
  cfg.hbf_page_buffers = 2;
  return cfg;
}

unsigned channel_of_subarray(unsigned subarray) {
  // Four channels own [0,2), [2,4), [4,6), and [6,8).
  return subarray / 2;
}

void drain(hbf_subarray_t *subarray) {
  while (!subarray->is_idle()) subarray->cycle();
}

bool valid(const hbf_phys_addr_t &phys) {
  return phys.subarray != ~0u && phys.block != ~0u && phys.page != ~0u;
}

bool same_phys(const hbf_phys_addr_t &lhs, const hbf_phys_addr_t &rhs) {
  return lhs.subarray == rhs.subarray && lhs.block == rhs.block &&
         lhs.page == rhs.page;
}

std::string write_temp_file(const char *contents) {
  char path[] = "/tmp/hbf-placement-XXXXXX";
  int fd = mkstemp(path);
  assert(fd >= 0);
  FILE *fp = fdopen(fd, "w");
  assert(fp != nullptr);
  assert(std::fputs(contents, fp) >= 0);
  assert(std::fclose(fp) == 0);
  return path;
}

void expect_bad_placement(const char *contents, unsigned channels) {
  std::string path = write_temp_file(contents);
  std::map<unsigned long long, unsigned> table;
  table[99] = 1;
  std::string error;
  assert(!hbf_load_placement_table(path.c_str(), channels, &table, &error));
  assert(!error.empty());
  // Failed parsing is transactional and must not expose a partial table.
  assert(table.size() == 1);
  assert(table[99] == 1);
  assert(unlink(path.c_str()) == 0);
}

void test_exact_media_tick_counts_and_buffer_invalidation() {
  memory_config cfg = make_config();
  hbf_subarray_t sa(0, &cfg);
  auto ticks_until_idle = [&sa]() {
    unsigned ticks = 0;
    while (!sa.is_idle()) { sa.cycle(); ++ticks; }
    return ticks;
  };
  sa.start_read(0, 0);
  assert(ticks_until_idle() == cfg.hbf_tR);
  sa.start_read(0, 0);
  assert(ticks_until_idle() == cfg.hbf_buffer_hit_latency);
  assert(sa.n_buffer_misses == 1 && sa.n_buffer_hits == 1);
  sa.start_program(1, 0);
  assert(ticks_until_idle() == cfg.hbf_tPROG);
  sa.start_erase(0);
  assert(ticks_until_idle() == cfg.hbf_tBERS);
  sa.start_read(0, 0);
  assert(ticks_until_idle() == cfg.hbf_tR);
  assert(sa.n_buffer_misses == 2);
}

void test_never_written_pages_have_unique_nonallocating_identity() {
  memory_config cfg = make_config();
  cfg.hbf_num_channels = 1;
  cfg.hbf_num_subarrays = 1;
  hbf_ftl_t ftl(&cfg);
  hbf_subarray_t subarray(0, &cfg);
  std::set<std::tuple<unsigned, unsigned, unsigned>> identities;

  for (unsigned long long logical_page = 0; logical_page < 64;
       ++logical_page) {
    hbf_phys_addr_t phys = ftl.translate(logical_page, false);
    assert(valid(phys));
    assert(phys.subarray == 0);
    assert((phys.block & (1u << 31)) != 0);
    assert(identities
               .insert(std::make_tuple(phys.subarray, phys.block, phys.page))
               .second);

    // The first access to each never-written page must miss. An immediate
    // duplicate must hit, proving the identity is stable as well as unique.
    subarray.start_read(phys.page, phys.block);
    drain(&subarray);
    subarray.start_read(phys.page, phys.block);
    drain(&subarray);
  }

  assert(subarray.n_buffer_misses == 64);
  assert(subarray.n_buffer_hits == 64);
  hbf_usage_info_t usage = ftl.get_usage();
  assert(usage.logical_pages == 0);
  assert(usage.total_blocks == 0);
  assert(usage.valid_pages == 0);
  assert(usage.subarrays_used == 0);
  assert(usage.histogram[0] == 1);

  // Real allocations occupy the low block-id namespace and cannot alias a
  // never-written page's stable identity.
  hbf_phys_addr_t pseudo = ftl.translate(3, false);
  hbf_phys_addr_t written = ftl.translate(3, true);
  assert(valid(written));
  assert((written.block & (1u << 31)) == 0);
  assert(!same_phys(pseudo, written));
  assert(same_phys(ftl.translate(3, false), written));
}

void test_writes_keep_per_subarray_active_blocks() {
  memory_config cfg = make_config();
  hbf_ftl_t ftl(&cfg);

  hbf_phys_addr_t first[8];
  std::set<unsigned> subarrays;
  for (unsigned long long logical_page = 0; logical_page < 8;
       ++logical_page) {
    first[logical_page] = ftl.translate(logical_page, true);
    assert(valid(first[logical_page]));
    assert(channel_of_subarray(first[logical_page].subarray) ==
           logical_page % cfg.hbf_num_channels);
    assert(first[logical_page].page == 0);
    subarrays.insert(first[logical_page].subarray);
  }
  assert(subarrays.size() == cfg.hbf_num_subarrays);

  // The next interleave round revisits each subarray's independent cursor.
  for (unsigned long long logical_page = 8; logical_page < 16;
       ++logical_page) {
    hbf_phys_addr_t phys = ftl.translate(logical_page, true);
    assert(valid(phys));
    unsigned channel = logical_page % cfg.hbf_num_channels;
    assert(channel_of_subarray(phys.subarray) == channel);
    unsigned first_index = (unsigned)(logical_page - 8);
    assert(phys.subarray == first[first_index].subarray);
    assert(phys.block == first[first_index].block);
    assert(phys.page == 1);
  }
  hbf_usage_info_t usage = ftl.get_usage();
  assert(usage.subarrays_used == cfg.hbf_num_subarrays);
  unsigned histogram_total = 0;
  for (unsigned bucket : usage.histogram) histogram_total += bucket;
  assert(histogram_total == cfg.hbf_num_subarrays);
}

void test_failed_overwrite_preserves_old_mapping() {
  memory_config cfg = make_config();
  cfg.hbf_num_channels = 1;
  cfg.hbf_num_subarrays = 1;
  cfg.hbf_pages_per_block = 1;
  cfg.hbf_size = cfg.hbf_page_size;  // exactly one physical block
  hbf_ftl_t ftl(&cfg);

  hbf_phys_addr_t original = ftl.translate(0, true);
  assert(valid(original));
  assert(ftl.is_block_erased(original.subarray, original.block));

  hbf_phys_addr_t failed = ftl.translate(0, true);
  assert(!valid(failed));
  assert(ftl.capacity_error());
  assert(same_phys(ftl.translate(0, false), original));
  hbf_usage_info_t usage = ftl.get_usage();
  assert(usage.logical_pages == 1);
  assert(usage.valid_pages == 1);
}

void test_fresh_and_recycled_block_erase_state() {
  memory_config cfg = make_config();
  cfg.hbf_num_channels = 1;
  cfg.hbf_num_subarrays = 1;
  hbf_ftl_t ftl(&cfg);

  hbf_phys_addr_t fresh = ftl.translate(0, true);
  assert(valid(fresh));
  assert(ftl.is_block_erased(fresh.subarray, fresh.block));

  ftl.invalidate(0);
  assert(!ftl.is_block_erased(fresh.subarray, fresh.block));
  hbf_phys_addr_t recycled = ftl.translate(1, true);
  assert(valid(recycled));
  assert(recycled.subarray == fresh.subarray);
  assert(recycled.block == fresh.block);
  assert(!ftl.is_block_erased(recycled.subarray, recycled.block));

  ftl.mark_block_erasing(recycled.subarray, recycled.block);
  ftl.mark_block_erased(recycled.subarray, recycled.block);
  assert(ftl.is_block_erased(recycled.subarray, recycled.block));
}

void test_ssd_gc_preserves_erased_free_block_state() {
  memory_config cfg = make_config();
  cfg.hbf_num_channels = 1;
  cfg.hbf_num_subarrays = 1;
  cfg.hbf_pages_per_block = 1;
  cfg.hbf_size = 2ull * cfg.hbf_page_size;
  cfg.hbf_media_mode = 1;
  cfg.hbf_overprovisioning = 0.5f;
  hbf_ftl_t ftl(&cfg);

  hbf_phys_addr_t victim = ftl.translate(0, true);
  assert(valid(victim));
  ftl.gc();

  hbf_phys_addr_t reused = ftl.translate(1, true);
  assert(valid(reused));
  assert(reused.block == victim.block);
  assert(ftl.is_block_erased(reused.subarray, reused.block));
}

void test_strict_placement_loader() {
  std::string valid_path = write_temp_file(
      "# page to channel\n0,0\n1 1\n1,1 # identical duplicate\n2,3\n");
  std::map<unsigned long long, unsigned> table;
  std::string error;
  assert(hbf_load_placement_table(valid_path.c_str(), 4, &table, &error));
  assert(error.empty());
  assert(table.size() == 3);
  assert(table[0] == 0 && table[1] == 1 && table[2] == 3);
  assert(unlink(valid_path.c_str()) == 0);

  expect_bad_placement("", 4);
  expect_bad_placement("not-a-page,0\n", 4);
  expect_bad_placement("0\n", 4);
  expect_bad_placement("0,1\n0,2\n", 4);
  expect_bad_placement("0,4\n", 4);

  table.clear();
  assert(!hbf_load_placement_table("/tmp/hbf-no-such-placement-table", 4,
                                   &table, &error));
  assert(!error.empty());
}

void test_explicit_placement_requires_every_requested_page() {
  std::string path = write_temp_file("0,0\n");
  memory_config cfg = make_config();
  cfg.hbf_num_channels = 1;
  cfg.hbf_num_subarrays = 1;
  cfg.hbf_placement_mode = 2;
  cfg.hbf_channel_map_file = const_cast<char *>(path.c_str());

  {
    hbf_ftl_t ftl(&cfg);
    assert(valid(ftl.translate(0, false)));
    assert(!valid(ftl.translate(1, false)));
    assert(ftl.mapping_error());
    assert(!valid(ftl.translate(1, true)));
    assert(ftl.mapping_error());
    assert(ftl.get_usage().logical_pages == 0);
  }
  assert(unlink(path.c_str()) == 0);
}

}  // namespace

int main(int argc, char **argv) {
  bool ran = false;
  if (argc == 1 || std::strcmp(argv[1], "pseudo") == 0) {
    test_exact_media_tick_counts_and_buffer_invalidation();
    test_never_written_pages_have_unique_nonallocating_identity();
    ran = true;
  }
  if (argc == 1 || std::strcmp(argv[1], "active-blocks") == 0) {
    test_writes_keep_per_subarray_active_blocks();
    ran = true;
  }
  if (argc == 1 || std::strcmp(argv[1], "overwrite") == 0) {
    test_failed_overwrite_preserves_old_mapping();
    ran = true;
  }
  if (argc == 1 || std::strcmp(argv[1], "erase-state") == 0) {
    test_fresh_and_recycled_block_erase_state();
    test_ssd_gc_preserves_erased_free_block_state();
    ran = true;
  }
  if (argc == 1 || std::strcmp(argv[1], "mapping") == 0) {
    test_strict_placement_loader();
    test_explicit_placement_requires_every_requested_page();
    ran = true;
  }
  if (!ran || argc > 2) {
    std::fprintf(stderr, "unknown test selector\n");
    return 2;
  }
  return 0;
}
