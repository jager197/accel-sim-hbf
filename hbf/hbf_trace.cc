#include "hbf_trace.h"

#include "mem_fetch.h"

#include <string.h>
#include <stdlib.h>
#include <string>

hbf_trace_t::hbf_trace_t(const char *path, int level)
    : m_file(NULL), m_diagnostic_file(NULL), m_level(level), m_next_request_id(1) {
  if (path != NULL && path[0] != '\0' && level > 0) {
    m_file = fopen(path, "w");
    if (m_file == NULL) {
      fprintf(stderr, "HBF trace: could not open %s\n", path);
      return;
    }
    fprintf(m_file,
            "sim_cycle,request_id,source_subpartition,op,address,page,channel,"
            "subarray,state,bytes,queue_depth,latency,cache_hit,mshr_hit,error\n");
    fflush(m_file);
    const char *diagnostics = getenv("HBF_DIAGNOSTICS");
    if (diagnostics != NULL && strcmp(diagnostics, "1") == 0) {
      std::string diagnostic_path = std::string(path) + ".diagnostics.csv";
      m_diagnostic_file = fopen(diagnostic_path.c_str(), "w");
      if (m_diagnostic_file == NULL) {
        fprintf(stderr, "HBF diagnostics: could not open %s\n",
                diagnostic_path.c_str());
        abort();
      }
      fprintf(m_diagnostic_file,
              "sim_cycle,request_id,mem_fetch_uid,created_cycle,state,op,"
              "address,channel,subarray,bytes\n");
    }
  }
}

hbf_trace_t::~hbf_trace_t() {
  if (m_file != NULL) fclose(m_file);
  if (m_diagnostic_file != NULL) fclose(m_diagnostic_file);
}

void hbf_trace_t::record(unsigned long long sim_cycle, const char *state,
                         mem_fetch *request, unsigned long long page,
                         unsigned channel, unsigned subarray, unsigned bytes,
                         unsigned queue_depth, unsigned long long latency,
                         bool cache_hit, bool mshr_hit, const char *error) {
  if (!enabled() || request == NULL) return;
  auto it = m_request_ids.find(request);
  if (it == m_request_ids.end()) {
    it = m_request_ids
             .insert(std::make_pair(request, m_next_request_id++))
             .first;
  }
  const char *op = request->get_is_write() ? "W" : "R";
  const char *err = (error != NULL && error[0] != '\0') ? error : "";
  fprintf(m_file,
          "%llu,%llu,%u,%s,%llu,%llu,%u,%u,%s,%u,%u,%llu,%u,%u,%s\n",
          sim_cycle, it->second, request->get_sub_partition_id(), op,
          (unsigned long long)request->get_addr(), page, channel, subarray,
          state != NULL ? state : "", bytes, queue_depth, latency,
          cache_hit ? 1 : 0, mshr_hit ? 1 : 0, err);
  if (m_diagnostic_file != NULL) {
    fprintf(m_diagnostic_file, "%llu,%llu,%u,%u,%s,%s,%llu,%u,%u,%u\n",
            sim_cycle, it->second, request->get_request_uid(),
            request->get_timestamp(), state != NULL ? state : "", op,
            (unsigned long long)request->get_addr(), channel, subarray, bytes);
  }
  if (m_level > 1) fflush(m_file);
  // mem_fetch objects are released after completion and their addresses can
  // be reused by a later request.  Retire the pointer key at the lifecycle
  // boundary so request IDs remain unique in long trace-driven runs.
  if (state != NULL && strcmp(state, "COMPLETED") == 0)
    m_request_ids.erase(request);
}
