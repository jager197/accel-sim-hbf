# LUD trace input

This small kernel comes from the Accel-Sim Rodinia trace collection:
`rodinia_2.0-ft/9.1/lud-rodinia-2.0-ft/_v__b__i___data_64_dat/traces/kernel-8.traceg`.
Only kernel 8 is replayed. The runner remaps global addresses into HBF and
checks all 158 request/completion pairs. The input trace is unchanged; the
kernel list selects just this kernel (host-copy records are not simulated).

Upstream trace collection: https://github.com/accel-sim/accel-sim-framework
