#!/bin/bash
rm -rf Results_AMD_Laptop/ Results_AMD_Laptop.7z
make clean

mkdir Results_AMD_Laptop
mkdir Results_AMD_Laptop/1024
mkdir Results_AMD_Laptop/4096
make -j$(nproc) >/dev/null

# Collect all the events we want to monitor (AMD compatible)
events="task-clock,context-switches,cpu-migrations,page-faults,instructions,cycles,\
stalled-cycles-frontend,branches,branch-misses,L1-dcache-load-misses,cpu/event=0x76,\
umask=0x01,name=backend_bound/,sched:sched_wakeup,sched:sched_wakeup_new"

# custom perf command
run_perf() {
  name=$1
  cmd=$2
  size=$3
  timestamp=$(date +"%Y%m%d%H%M%S") # 每次运行生成不同时间戳
  echo "Running $name test with size $size..."
  sudo perf stat -r 10 -e $events -- $cmd $size 2>&1 | tee Results_AMD_Laptop/$size/perfStats_${name}_$timestamp.txt
}

# Run perf on all approaches for different sizes
for size in 1024 4096; do
  run_perf "single" ./matrix_mul_single $size
  run_perf "double" ./matrix_mul_double $size
  run_perf "parallel_standard" ./parallel_matrix_mul_standard $size
  run_perf "fifo" ./matrix_mul_fifo $size
  run_perf "lifo" ./matrix_mul_lifo $size
  run_perf "async" ./matrix_mul_async $size
  run_perf "tbb" ./matrix_mul_tbb $size
  run_perf "openBLAS" ./matrix_mul_blas $size
  run_perf "openMP" ./matrix_mul_openmp $size
done

# Run test on all results
echo "Validating the results"
for size in 1024 4096; do
  for variant in async fifo lifo tbb standard openBLAS openMP; do
    if [ "$variant" != "openBLAS" ]; then
      diff -u <(sort matrix_mul_single_${size}.txt) <(sort parallel_matrix_mul_${variant}_${size}.txt) |
        grep -E "^[+|-]" | grep -v "+++" >Results_AMD_Laptop/$size/diff_parallel_${variant}_$(date +"%Y%m%d%H%M%S").txt
      if [ $(wc -c <Results_AMD_Laptop/$size/diff_parallel_${variant}_*.txt) -eq 0 ]; then
        echo -e "\e[32mValidation Passed for $variant with size $size\e[0m"
        rm -rf Results_AMD_Laptop/$size/diff_parallel_${variant}_*.txt
      else
        echo -e "\e[31mValidation Failed for $variant with size $size\e[0m"
      fi
    else
      diff -u <(sort matrix_mul_double_${size}.txt) <(sort parallel_matrix_mul_${variant}_${size}.txt) |
        grep -E "^[+|-]" | grep -v "+++" >Results_AMD_Laptop/$size/diff_${variant}_$(date +"%Y%m%d%H%M%S").txt
      if [ $(wc -c <Results_AMD_Laptop/$size/diff_${variant}_*.txt) -eq 0 ]; then
        echo -e "\e[32mValidation Passed for $variant with size $size\e[0m"
        rm -rf Results_AMD_Laptop/$size/diff_${variant}_*.txt
      else
        echo -e "\e[31mValidation Failed for $variant with size $size\e[0m"
      fi
    fi
  done
done

# Cleanup
make clean
rm -rf matrix_mul_*.txt parallel_matrix_mul_*.txt

# Zip results
sudo 7z a -t7z -mx=9 -mmt=on -m0=lzma2 -md=1024m -ms=on Results_AMD_Laptop.7z Results_AMD_Laptop/

