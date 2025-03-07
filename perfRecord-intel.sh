#!/bin/bash
rm -rf Results_Intel/ Results_Intel.7z
make clean

mkdir Results_Intel
mkdir Results_Intel/1024
mkdir Results_Intel/4096
mkdir Results_Intel/8192
make -j$(nproc) >/dev/null

# Collect all the events we want to monitor (AMD compatible)
events="duration_time,user_time,system_time,\
instructions,branch-instructions,branch-misses,\
cache-references,cache-misses,cpu-cycles,ref-cycles,\
topdown-fetch-bubbles,topdown-recovery-bubbles,topdown-slots-issued,topdown-slots-retired,topdown-total-slots,\
cstate_core/c3-residency/,cstate_core/c6-residency/,cstate_core/c7-residency/,\
cstate_pkg/c2-residency/,cstate_pkg/c3-residency/,cstate_pkg/c6-residency/,cstate_pkg/c7-residency/,cstate_pkg/c8-residency/,cstate_pkg/c9-residency/,cstate_pkg/c10-residency/,\
power/energy-cores/,power/energy-pkg/,power/energy-ram/,\
uncore_imc/data_reads/,uncore_imc/data_writes/"

# custom perf command
run_perf() {
  name=$1
  cmd=$2
  size=$3
  timestamp=$(date +"%Y%m%d%H%M%S") # 每次运行生成不同时间戳
  echo "Running $name test with size $size..."
  sudo perf stat -r 10 -e $events -- $cmd $size 2>&1 | tee Results_Intel/$size/perfStats_${name}_$timestamp.txt
}

# Run perf on all approaches for different sizes
for size in 1024 4096 8192; do
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
for size in 1024 4096 8192; do
  for variant in async fifo lifo tbb standard openBLAS openMP; do
    if [ "$variant" != "openBLAS" ]; then
      diff -u <(sort matrix_mul_single_${size}.txt) <(sort parallel_matrix_mul_${variant}_${size}.txt) |
        grep -E "^[+|-]" | grep -v "+++" >Results_Intel/$size/diff_parallel_${variant}_$(date +"%Y%m%d%H%M%S").txt
      if [ $(wc -c <Results_Intel/$size/diff_parallel_${variant}_*.txt) -eq 0 ]; then
        echo -e "\e[32mValidation Passed for $variant with size $size\e[0m"
        rm -rf Results_Intel/$size/diff_parallel_${variant}_*.txt
      else
        echo -e "\e[31mValidation Failed for $variant with size $size\e[0m"
      fi
    else
      diff -u <(sort matrix_mul_double_${size}.txt) <(sort parallel_matrix_mul_${variant}_${size}.txt) |
        grep -E "^[+|-]" | grep -v "+++" >Results_Intel/$size/diff_${variant}_$(date +"%Y%m%d%H%M%S").txt
      if [ $(wc -c <Results_Intel/$size/diff_${variant}_*.txt) -eq 0 ]; then
        echo -e "\e[32mValidation Passed for $variant with size $size\e[0m"
        rm -rf Results_Intel/$size/diff_${variant}_*.txt
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
sudo 7z a -t7z -mx=9 -mmt=on -m0=lzma2 -md=1024m -ms=on Results_Intel.7z Results_Intel/
