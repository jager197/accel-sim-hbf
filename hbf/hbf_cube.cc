#include "hbf_cube.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include "gpu-sim.h"
#include "hbf_controller.h"
#include "mem_fetch.h"

hbf_cube_t::hbf_cube_t(const memory_config *config, memory_stats_t *stats,
                       gpgpu_sim *gpu, unsigned stack_id,
                       unsigned total_stacks,
                       unsigned long long stack_capacity)
    : m_stack_config(new memory_config(*config)),
      m_controller(NULL),
      m_return_capacity(config->gpgpu_dram_return_queue_size > 0
                            ? config->gpgpu_dram_return_queue_size
                            : 1),
      m_return_count(0),
      m_trace_path(NULL),
      m_stack_id(stack_id),
      m_stack_capacity(stack_capacity == 0 ? config->hbf_size
                                           : stack_capacity) {
  // A controller reads capacity through memory_config.  Give every stack a
  // private read-only view so its FTL enforces per-stack rather than system
  // capacity while all other timing/resource knobs retain per-stack meaning.
  m_stack_config->hbf_size = m_stack_capacity;
  // The per-source routing queues collectively model one finite return
  // buffer.  Keep the controller FIFO and this second-stage buffer aligned.
  m_stack_config->gpgpu_dram_return_queue_size = m_return_capacity;

  // Independent trace writers cannot safely target the same path.  Preserve
  // the v1 schema while splitting multi-stack traces into separate files.
  if (total_stacks > 1 && config->hbf_trace_file != NULL &&
      config->hbf_trace_file[0] != '\0') {
    int path_len = snprintf(NULL, 0, "%s.stack%u.csv", config->hbf_trace_file,
                            stack_id);
    m_trace_path = new char[path_len + 1];
    snprintf(m_trace_path, path_len + 1, "%s.stack%u.csv",
             config->hbf_trace_file, stack_id);
    m_stack_config->hbf_trace_file = m_trace_path;
  }

  m_controller =
      new hbf_controller_t(stack_id, m_stack_config, stats, NULL, gpu);
}

hbf_cube_t::~hbf_cube_t() {
  // The controller owns no mem_fetch objects that have not already been
  // returned.  Any queued completion is still owned by the simulator; drain
  // the routing queues before releasing the controller metadata.
  for (auto &entry : m_returns) entry.second.clear();
  m_return_count = 0;
  delete m_controller;
  delete[] m_trace_path;
  delete m_stack_config;
}

bool hbf_cube_t::full(bool is_write) const {
  return m_controller->full(is_write);
}

void hbf_cube_t::push(mem_fetch *request) { m_controller->push(request); }

void hbf_cube_t::collect_returns() {
  while (m_return_count < m_return_capacity) {
    mem_fetch *request = m_controller->return_queue_top();
    if (request == NULL) break;
    m_controller->return_queue_pop();
    m_returns[request->get_sub_partition_id()].push_back(request);
    ++m_return_count;
  }
}

void hbf_cube_t::cycle() {
  m_controller->cycle();
  collect_returns();
}

mem_fetch *hbf_cube_t::pop_return_for(unsigned global_subpartition_id) {
  auto it = m_returns.find(global_subpartition_id);
  if (it == m_returns.end() || it->second.empty()) return NULL;
  mem_fetch *request = it->second.front();
  it->second.pop_front();
  assert(m_return_count > 0);
  --m_return_count;
  if (it->second.empty()) m_returns.erase(it);
  return request;
}

bool hbf_cube_t::busy() const {
  return m_controller->has_work() || !m_returns.empty();
}

unsigned hbf_cube_t::num_channels() const {
  return m_controller->get_num_channels();
}

void hbf_cube_t::print_stat(FILE *fp) { m_controller->print_stat(fp); }
