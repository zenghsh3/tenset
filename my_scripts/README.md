## Environments
- CPU

## How to run
1. build and install tvm with this repo (add printing info based on `tenset` repo)
2. run following commands:
```
cd my_scripts
python tune_and_compile.py > log
```

## 结果
- extract_tasks (include simple tasks)
  - 总共有71个tasks（包括重复）
- dispatch.query (ApplyHistoryBest)
  - PREPARE_LAYOUT_REWRITE 阶段，同样总共有71个tasks（tasks集合和extract_tasks阶段是相同的，但顺序不一致，原因是?）
  - final build 阶段 (TracingEnv is None)，同样总共有71个tasks（tasks集合和extract_tasks阶段是相同的，且顺序相同【注意：如果运行tuning的话，会对dag进行layout rewrite，会产生new_key】）
