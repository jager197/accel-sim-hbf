// HBF Phase 3 (v0.4): Host Channel Model
//
// OCP HBF v0.7.0 §4.3/§4.5: each HBF stack exposes up to 16 independent host
// channels (UCIe 3.0 links). Each channel owns a FIXED set of NAND dies
// (here: a contiguous slice of the sub-array array), has its own linear
// local address space, and requests from one channel can never touch the
// resources of another channel. Channels are independently clocked and
// asynchronous.
//
// This class models one channel's identity, its NAND resource slice, its
// interface bandwidth (the UCIe/AXI link), and its per-channel accounting
// (outstanding writes per OCP §5.4.1.7, transfer credits). The MSHR and
// write buffer remain shared structures in the controller (a documented
// simplification: they are tagged per channel), while scheduling and
// delivery are channel-affine.

#ifndef HBF_CHANNEL_H
#define HBF_CHANNEL_H

class hbf_channel_t {
 public:
  hbf_channel_t(unsigned id, unsigned first_subarray, unsigned num_subarrays,
                double bw_bytes_per_tick)
      : m_id(id),
        m_first_subarray(first_subarray),
        m_num_subarrays(num_subarrays),
        m_bw_bytes_per_tick(bw_bytes_per_tick),
        m_credit_bytes(0.0),
        m_max_credit_bytes(bw_bytes_per_tick * 512.0) {  // ~512-tick burst cap
    n_requests = 0;
    n_page_reads = 0;
    n_page_programs = 0;
    n_block_erases = 0;
    n_outstanding_writes = 0;
    n_writes_deferred = 0;
    n_transfer_stall_ticks = 0;
  }

  unsigned get_id() const { return m_id; }
  unsigned get_first_subarray() const { return m_first_subarray; }
  unsigned get_num_subarrays() const { return m_num_subarrays; }

  // ── NAND resource affinity ───────────────────────────────────────────
  // Is this sub-array part of this channel's die set?
  bool owns_subarray(unsigned subarray_id) const {
    return subarray_id >= m_first_subarray &&
           subarray_id < m_first_subarray + m_num_subarrays;
  }
  // Map a channel-local sub-array index to a global sub-array id.
  unsigned subarray_of_local(unsigned local_idx) const {
    return m_first_subarray + (local_idx % m_num_subarrays);
  }

  // ── Interface transfer credit (UCIe/AXI link bandwidth) ──────────────
  // Each tick the channel's link can carry at most m_bw_bytes_per_tick
  // bytes (spec Table 2: 256 GB/s raw x 75% AXI efficiency per channel).
  // Returns true if a transfer of `bytes` is allowed now, and consumes
  // the credit. Credits accumulate up to a burst cap.
  bool try_consume_credit(double bytes) {
    if (bytes > m_credit_bytes) return false;
    m_credit_bytes -= bytes;
    return true;
  }
  void replenish_credit() {
    m_credit_bytes += m_bw_bytes_per_tick;
    if (m_credit_bytes > m_max_credit_bytes) m_credit_bytes = m_max_credit_bytes;
  }
  bool credit_exhausted() const { return m_credit_bytes <= 0.0; }

  // ── Outstanding-write accounting (OCP §5.4.1.7) ──────────────────────
  // Products limit outstanding write requests per host channel (e.g. 64 or
  // 128). Writes beyond the limit must wait (the controller defers flushes).
  bool can_accept_write(unsigned max_outstanding_writes) const {
    return n_outstanding_writes < max_outstanding_writes;
  }

  // Statistics
  unsigned long long n_requests;
  unsigned long long n_page_reads;
  unsigned long long n_page_programs;
  unsigned long long n_block_erases;
  unsigned n_outstanding_writes;
  unsigned long long n_writes_deferred;
  unsigned long long n_transfer_stall_ticks;

 private:
  unsigned m_id;
  unsigned m_first_subarray;  // first global sub-array id owned by this channel
  unsigned m_num_subarrays;   // number of sub-arrays owned (its die set)
  double m_bw_bytes_per_tick; // interface bytes per DRAM tick
  double m_credit_bytes;      // available transfer credit
  double m_max_credit_bytes;  // credit burst cap
};

#endif  // HBF_CHANNEL_H
