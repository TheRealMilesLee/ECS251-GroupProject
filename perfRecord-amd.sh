#!/bin/bash
rm -rf Results/ Results.7z
make clean

mkdir Results
make -j$(nproc) >/dev/null

# Collect all the events we want to monitor (AMD compatible)
events="task-clock,context-switches,cpu-migrations,page-faults,instructions,cycles,\
stalled-cycles-frontend,branches,branch-misses,L1-dcache-load-misses,cpu/event=0x76,\
umask=0x01,name=backend_bound/,sched:sched_wakeup,sched:sched_wakeup_new"

# custom perf command
run_perf() {
  name=$1
  cmd=$2
  timestamp=$(date +"%Y%m%d%H%M%S") # 每次运行生成不同时间戳
  echo "Running $name test..."
  sudo perf stat -r 10 -e $events -- $cmd 2>&1 | tee Results/perfStats_${name}_$timestamp.txt
}

# Run perf on all approaches
run_perf "single" ./matrix_mul_single
run_perf "double" ./matrix_mul_double
run_perf "parallel_standard" ./parallel_matrix_mul_standard
run_perf "fifo" ./matrix_mul_fifo
run_perf "lifo" ./matrix_mul_lifo
run_perf "async" ./matrix_mul_async
run_perf "tbb" ./matrix_mul_tbb
run_perf "openBLAS" ./matrix_mul_blas
run_perf "openMP" ./matrix_mul_openmp

# Run test on all results
echo "Validating the results"
for variant in async fifo lifo tbb standard openBLAS openMP; do
  if [ "$variant" != "openBLAS" ]; then
    diff -u <(sort matrix_mul_single.txt) <(sort parallel_matrix_mul_${variant}.txt) |
      grep -E "^[+|-]" | grep -v "+++" >Results/diff_parallel_${variant}_$(date +"%Y%m%d%H%M%S").txt
    if [ $(wc -c <Results/diff_parallel_${variant}_*.txt) -eq 0 ]; then
      echo -e "\e[32mValidation Passed for $variant\e[0m"
      rm -rf Results/diff_parallel_${variant}_*.txt
    else
      echo -e "\e[31mValidation Failed for $variant\e[0m"
    fi
  else
    diff -u <(sort matrix_mul_double.txt) <(sort parallel_matrix_mul_${variant}.txt) |
      grep -E "^[+|-]" | grep -v "+++" >Results/diff_${variant}_$(date +"%Y%m%d%H%M%S").txt
    if [ $(wc -c <Results/diff_${variant}_*.txt) -eq 0 ]; then
      echo -e "\e[32mValidation Passed for $variant\e[0m"
      rm -rf Results/diff_${variant}_*.txt
    else
      echo -e "\e[31mValidation Failed for $variant\e[0m"
    fi
  fi
done

# Cleanup
make clean
rm -rf matrix_mul_*.txt parallel_matrix_mul_*.txt

# Zip results
sudo 7z a -t7z -mx=9 -mmt=on -m0=lzma2 -md=1024m -ms=on Results.7z Results/
