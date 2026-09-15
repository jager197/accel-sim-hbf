# Placement case study

Run the full 24-cell matrix with the current installed HBF simulator:

```bash
python3 experiments/18_placement_cases/run.py --tag NEW_TAG --jobs 4
```

Every tag is new. Three locality densities (1/8/32 distinct 128 B entries per
page), four mappings, and two read timings (20/12750 DRAM ticks) use the same
16-page GPU workload. Four mappings are interleave, two-page grouping,
capacity-contiguous (the active pages all fall on channel 0), and balanced
four-page regions via an explicit table. All use demand reads, MSHR merging,
256-operation admission, four channels, 32 subarrays and eight active slots.

The runner checks installed source consistency, hashes binaries/inputs, pairs
native creation/INGRESS/COMPLETED events, and verifies actual page ownership.
Page services include NAND page-buffer hits; array reads exclude them.
Frozen paper results are `artifact/reference/placement_cases.csv`. The revision audit
also confirms the same requested address/byte multiset across mappings and
exact reproduction of the original nine shortened-timing cells.

The 20260909-cases run uses the same simulator library as the confirmed
admission and phase-boundary families. It changes no controller mechanism.
