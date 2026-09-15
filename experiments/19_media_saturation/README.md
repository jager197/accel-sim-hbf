# C++ media saturation with sufficient GPU page-level concurrency

This suite keeps the existing GPU simulator, controller and backpressure path.
Each active lane loads one word from a different 4 KiB page. The coalescer emits
one 32 B sector per lane. Every request is therefore a unique NAND page read;
page cache and NAND buffer-hit shortcuts are disabled. No Python performance
model is involved.

```bash
python3 experiments/19_media_saturation/run.py --tag NEW_TAG --jobs 4
```

Default matrix (nine runs):

- 1/2/4/8/16 channels; 32 subarrays and active-operation slots per channel;
  1024 distinct pages and 1024 independent lane producers.
- 16 channels: 256/512 producers for the same 1024 pages. The former is a
  declared supply-limited negative control; the latter tests near-saturation.
- 16 channels: 2048 pages/producers, checking a longer saturated stream.
- 16 channels with total subarrays/slots held at32, isolating resource count
  from channel count.

All runs retain default15/200/2000us media timing,80 SMs,32 memory partitions,
192 GB/s link per channel and4096 controller-operation admission slots.
Requests remain subject to the normal GPU pipeline and return path.

Reported quantities are distinct:

1. Page-read throughput = unique page READ operations / first-ingress-to-last-
   controller-return interval.
2. Array-service bandwidth = throughput ×4096 B. These bytes are sensed inside
   NAND; they are not all returned to the GPU.
3. Returned-sector bandwidth = throughput ×32 B.
4. Useful scalar-load bandwidth = throughput ×4 B.

The ratios are fixed at128:1 between array bytes and returned sectors, and
8:1 between sector bytes and requested scalar words. This is a media resource
scaling probe, not full-page application throughput or a latency-hiding claim.

The analyzer verifies exact trace schema, unique request/page identities,
32 B requests, page-to-channel balance, actual subarray use, non-overlapping
media intervals, configured READ duration, completion order, peak active slots,
and mean media busy fraction including startup/tail. Occupancy is reconstructed
from actual READ issue events and configured tR; it is not a hardware counter.
Saturation is checked against the known configured service bound and by varying
producer count and stream length. Failed or partial matrices have no final CSV.

Runs preserve code/config/binary hashes and raw traces. `--cases` accepts only
names in the declared matrix; it creates a separately tagged diagnostic subset.
