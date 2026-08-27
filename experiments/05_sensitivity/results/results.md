# 敏感性 / 消融

## A. 机制消融（kv_swa N=32 D=8 W=4）

| Variant | cycles | page reads | cache hits | MSHR hits |
|---:|---:|---:|---:|---:|
| Full HBF | 58,909 | 1 | 85 | 258 |
| no MSHR | 59,129 | 1 | 343 | 0 |
| no page cache | 55,757 | 86 | 0 | 258 |
| no page buffer | 58,909 | 1 | 85 | 258 |
| serial (max_active=1) | 58,909 | 1 | 85 | 258 |

> 注: 有复用负载里页缓存吸收了大量重复读，消融的主要差异体现在页读/缓存命中数上；tR 与并行度的效果在无复用流式负载上扫描。

## B. 参数扫描

| Param | value | workload | cycles |
|---:|---:|---|---:|
| cache | 0 | kv_swa N=32 (reuse-heavy) | 55,757 |
| cache | 64 | kv_swa N=32 (reuse-heavy) | 58,909 |
| cache | 256 | kv_swa N=32 (reuse-heavy) | 58,909 |
| cache | 1024 | kv_swa N=32 (reuse-heavy) | 58,909 |
| tR | 10000 | spill tier, kv_swa N=24 (serial, no caches) | 2,905,372 |
| tR | 20000 | spill tier, kv_swa N=24 (serial, no caches) | 5,781,981 |
| tR | 40000 | spill tier, kv_swa N=24 (serial, no caches) | 11,535,204 |
| active | 8 | weight_load 4MB (streaming, no reuse) | 354,018 |
| active | 64 | weight_load 4MB (streaming, no reuse) | 290,888 |
| active | 256 | weight_load 4MB (streaming, no reuse) | 290,888 |
