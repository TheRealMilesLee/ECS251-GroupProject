#!/bin/bash
rm -rf Results/ Results.7z
make clean

mkdir Results
make -j$(sysctl -n hw.ncpu) >/dev/null

# Define dtrace script for basic performance monitoring
dtrace_script="profile-10ms /execname == \"matrix_mul_single\"/ { @[probefunc] = count(); }"

# Custom dtrace command
run_dtrace() {
  name=$1
  cmd=$2
  timestamp=$(date +"%Y%m%d%H%M%S") # 每次运行生成不同时间戳
  echo "Running $name test..."
  sudo dtrace -s $dtrace_script >Results/dtrace_${name}_$timestamp.txt & # Start dtrace in background
  $cmd                                                                   # Execute the command
  kill $!                                                                # Kill the dtrace process after the command finishes
}

# Run dtrace on all approaches
run_dtrace "single" ./matrix_mul_single
run_dtrace "double" ./matrix_mul_double
run_dtrace "parallel_standard" ./parallel_matrix_mul_standard
run_dtrace "fifo" ./matrix_mul_fifo
run_dtrace "lifo" ./matrix_mul_lifo
run_dtrace "async" ./matrix_mul_async
run_dtrace "tbb" ./matrix_mul_tbb
run_dtrace "openBLAS" ./matrix_mul_blas
run_dtrace "openMP" ./matrix_mul_openmp

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
