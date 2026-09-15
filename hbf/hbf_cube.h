// One logical HBF cube shared by all GPU memory partitions.
//
// A cube owns one controller and therefore one global channel namespace,
// page cache, FTL, ingress path, and completion path.  GPU partitions retain
// their source subpartition id on each mem_fetch; the cube uses that id only
// to route a completed request back to the correct partition.

#ifndef HBF_CUBE_H
#define HBF_CUBE_H

#include <deque>
#include <map>
#include <stdio.h>

class hbf_controller_t;
class memory_config;
class memory_stats_t;
class gpgpu_sim;
class mem_fetch;

class hbf_cube_t {
 public:
  hbf_cube_t(const memory_config *config, memory_stats_t *stats,
             gpgpu_sim *gpu, unsigned stack_id = 0,
             unsigned total_stacks = 1,
             unsigned long long stack_capacity = 0);
  ~hbf_cube_t();

  bool full(bool is_write) const;
  void push(mem_fetch *request);

  // Advance the device once per DRAM tick.  The GPU-level HBF system owns the
  // clocking responsibility; memory partitions only inject/drain traffic.
  void cycle();

  // Returns one completion for a global GPU subpartition, or NULL when that
  // source has no ready completion.  The controller's FIFO is drained into
  // per-source queues in cycle(), avoiding head-of-line blocking between
  // memory partitions.
  mem_fetch *pop_return_for(unsigned global_subpartition_id);

  bool busy() const;
  unsigned num_channels() const;
  unsigned stack_id() const { return m_stack_id; }
  unsigned long long capacity() const { return m_stack_capacity; }
  void print_stat(FILE *fp);

 private:
  void collect_returns();

  memory_config *m_stack_config;
  hbf_controller_t *m_controller;
  std::map<unsigned, std::deque<mem_fetch *> > m_returns;
  unsigned m_return_capacity;
  unsigned m_return_count;
  char *m_trace_path;
  unsigned m_stack_id;
  unsigned long long m_stack_capacity;
};

#endif  // HBF_CUBE_H
