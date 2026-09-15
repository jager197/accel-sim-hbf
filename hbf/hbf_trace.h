// Request-level HBF trace writer.

#ifndef HBF_TRACE_H
#define HBF_TRACE_H

#include <map>
#include <stdio.h>

class mem_fetch;

class hbf_trace_t {
 public:
  hbf_trace_t(const char *path, int level);
  ~hbf_trace_t();

  bool enabled() const { return m_file != NULL && m_level > 0; }

  bool diagnostics_enabled() const { return m_diagnostic_file != NULL; }

  // Emit one lifecycle event.  `latency` is in the simulator's core-cycle
  // unit when known; zero is used for admission/issue events.
  void record(unsigned long long sim_cycle, const char *state,
              mem_fetch *request, unsigned long long page, unsigned channel,
              unsigned subarray, unsigned bytes, unsigned queue_depth,
              unsigned long long latency, bool cache_hit, bool mshr_hit,
              const char *error);

 private:
  FILE *m_file;
  FILE *m_diagnostic_file;
  int m_level;
  unsigned long long m_next_request_id;
  std::map<const mem_fetch *, unsigned long long> m_request_ids;
};

#endif  // HBF_TRACE_H
