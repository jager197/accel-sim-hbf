#include "hbf_system.h"

#include <assert.h>
#include <limits.h>
#include <stdlib.h>

#include "gpu-sim.h"
#include "hbf_cube.h"
#include "mem_fetch.h"

hbf_system_t::hbf_system_t(const memory_config *config,
                           memory_stats_t *stats, gpgpu_sim *gpu)
    : m_config(config),
      m_num_stacks(config->hbf_num_stacks > 0 ? config->hbf_num_stacks : 1),
      m_stack_map(config->hbf_stack_map == 1 ? 1 : 0),
      m_rewrite_addresses(m_num_stacks > 1 || config->hbf_route_all),
      m_total_capacity(config->hbf_size),
      m_stack_capacity(config->hbf_size / m_num_stacks) {
  if (!capacity_has_equal_whole_blocks(
          m_total_capacity, config->hbf_page_size,
          config->hbf_pages_per_block, m_num_stacks)) {
    fprintf(stderr,
            "HBF: total capacity (%llu bytes) must contain an equal, "
            "erase-block-aligned capacity for each of %u stacks "
            "(page=%u bytes, pages/block=%u)\n",
            m_total_capacity, m_num_stacks, config->hbf_page_size,
            config->hbf_pages_per_block);
    exit(1);
  }
  if (m_num_stacks > 1 && config->hbf_placement_mode == 2) {
    fprintf(stderr,
            "HBF: explicit page,channel placement tables do not encode a "
            "stack; use stack_map 0/1 for multi-stack runs\n");
    exit(1);
  }

  m_stacks.reserve(m_num_stacks);
  m_routed_requests.assign(m_num_stacks, 0);
  m_completed_requests.assign(m_num_stacks, 0);
  for (unsigned stack = 0; stack < m_num_stacks; ++stack) {
    m_stacks.push_back(new hbf_cube_t(config, stats, gpu, stack,
                                      m_num_stacks, m_stack_capacity));
  }
}

hbf_system_t::~hbf_system_t() {
  // Outstanding requests are still simulator-owned.  Restore their externally
  // visible addresses before releasing device metadata.
  for (auto &entry : m_original_addresses)
    entry.first->set_addr(entry.second);
  m_original_addresses.clear();
  for (hbf_cube_t *stack : m_stacks) delete stack;
}

hbf_stack_location_t hbf_system_t::locate(
    unsigned long long address) const {
  const unsigned long long logical = logical_address_for_mapping(
      address, m_config->hbf_base_addr, m_config->hbf_route_all);
  return map_relative_address(logical, m_total_capacity,
                              m_config->hbf_page_size, m_num_stacks,
                              m_stack_map);
}

bool hbf_system_t::full(unsigned long long address, bool is_write) const {
  hbf_stack_location_t location = locate(address);
  assert(location.stack < m_stacks.size());
  return m_stacks[location.stack]->full(is_write);
}

void hbf_system_t::push(mem_fetch *request) {
  assert(request != NULL);
  hbf_stack_location_t location = locate(request->get_addr());
  assert(location.stack < m_stacks.size());

  if (m_rewrite_addresses) {
    const unsigned long long original_address = request->get_addr();
    bool inserted =
        m_original_addresses.insert(std::make_pair(request, original_address))
            .second;
    assert(inserted && "HBF request was pushed more than once");
    if (location.local_offset > ULLONG_MAX - m_config->hbf_base_addr) {
      fprintf(stderr, "HBF: stack-local address overflows GPU address space\n");
      exit(1);
    }
    request->set_addr(m_config->hbf_base_addr + location.local_offset);
  }

  m_routed_requests[location.stack]++;
  m_stacks[location.stack]->push(request);
}

void hbf_system_t::cycle() {
  for (hbf_cube_t *stack : m_stacks) stack->cycle();
}

void hbf_system_t::restore_address(mem_fetch *request) {
  if (!m_rewrite_addresses || request == NULL) return;
  auto original = m_original_addresses.find(request);
  assert(original != m_original_addresses.end() &&
         "HBF completion has no saved GPU address");
  request->set_addr(original->second);
  m_original_addresses.erase(original);
}

mem_fetch *hbf_system_t::pop_return_for(unsigned global_subpartition_id) {
  unsigned &next = m_return_rr_next[global_subpartition_id];
  if (next >= m_num_stacks) next = 0;
  for (unsigned checked = 0; checked < m_num_stacks; ++checked) {
    unsigned stack = (next + checked) % m_num_stacks;
    mem_fetch *request =
        m_stacks[stack]->pop_return_for(global_subpartition_id);
    if (request == NULL) continue;
    next = (stack + 1) % m_num_stacks;
    m_completed_requests[stack]++;
    restore_address(request);
    return request;
  }
  return NULL;
}

bool hbf_system_t::busy() const {
  for (const hbf_cube_t *stack : m_stacks)
    if (stack->busy()) return true;
  return !m_original_addresses.empty();
}

unsigned hbf_system_t::channels_per_stack() const {
  return m_stacks.empty() ? 0 : m_stacks.front()->num_channels();
}

void hbf_system_t::print_stat(FILE *fp) {
  fprintf(fp, "\n========= HBF System Statistics =========\n");
  fprintf(fp, "HBF Stacks:             %u\n", m_num_stacks);
  fprintf(fp, "HBF Stack Map:          %s\n",
          m_stack_map == 1 ? "contiguous" : "page-interleave");
  fprintf(fp, "HBF Total Capacity:     %llu bytes\n", m_total_capacity);
  fprintf(fp, "HBF Per-Stack Capacity: %llu bytes\n", m_stack_capacity);
  fprintf(fp, "HBF Channels Per Stack: %u\n", channels_per_stack());
  for (unsigned stack = 0; stack < m_num_stacks; ++stack) {
    fprintf(fp,
            "\n--------- HBF Stack %u (routed=%llu completed=%llu) ---------\n",
            stack, m_routed_requests[stack], m_completed_requests[stack]);
    m_stacks[stack]->print_stat(fp);
  }
}
