#!/bin/bash
rm -rf Results/
rm -rf Results.7z
make clean

mkdir Results
make

timestamp=$(date +"%Y%m%d%H%M%S")

# Collect all the events we want to monitor
events="task-clock,context-switches,cpu-migrations,page-faults,instructions,cycles,stalled-cycles-frontend,stalled-cycles-backend,branches,branch-misses,L1-dcache-load-misses,LLC-load-misses"

# custom perf command
run_perf() {
  name=$1
  cmd=$2
  echo "Running $name test..."
  perf stat -e $events -a -- $cmd 2>Results/perfStats_${name}_$timestamp.txt
}

# run perf on all approaches
run_perf "single" ./matrix_mul_single
run_perf "double" ./matrix_mul_double
run_perf "parallel_standard" ./parallel_matrix_mul_standard
run_perf "fifo" ./matrix_mul_fifo
run_perf "lifo" ./matrix_mul_lifo
run_perf "async" ./matrix_mul_async
run_perf "tbb" ./matrix_mul_tbb
run_perf "openBLAS" ./matrix_mul_blas
run_perf "openMP" ./matrix_mul_openmp

# run test on all results
echo "Validating the results"
for variant in async fifo lifo tbb standard openBLAS openMP; do
  if [ $variant != "openBLAS" ]; then
    diff -u matrix_mul_single.txt parallel_matrix_mul_${variant}.txt | grep -E "^\\+" | grep -v "+++" >Results/diff_parallel_${variant}_$timestamp.txt
    if [ $(wc -c <Results/diff_parallel_${variant}_$timestamp.txt) -eq 0 ]; then
      echo -e "\e[32mValidation Passed for $variant\e[0m"
      rm -rf Results/diff_parallel_${variant}_$timestamp.txt
    else
      echo -e "\e[31mValidation Failed for $variant\e[0m"
    fi
  else
    diff -u matrix_mul_double.txt parallel_matrix_mul_${variant}.txt | grep -E "^\\+" | grep -v "+++" >Results/diff_${variant}_$timestamp.txt
    if [ $(wc -c <Results/diff_${variant}_$timestamp.txt) -eq 0 ]; then
      echo -e "\e[32mValidation Passed for $variant\e[0m"
      rm -rf Results/diff_${variant}_$timestamp.txt
    else
      echo -e "\e[31mValidation Failed for $variant\e[0m"
    fi
  fi
done

# cleanup
make clean
rm -rf matrix_mul_*.txt
rm -rf parallel_matrix_mul_*.txt

# zip
sudo 7z a -t7z -mx=9 -mmt=on -m0=lzma2 -md=1024m -ms=on Results.7z Results/
