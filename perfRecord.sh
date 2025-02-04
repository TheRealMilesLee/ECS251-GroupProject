#!/bin/bash

make clean
make

timestamp=$(date +"%Y%m%d%H%M%S")

# Start the perf stat for the matrix_mul_single
perf stat ./matrix_mul_single 2>Results/perfStats_single_$timestamp.txt

# Start the perf stat for the matrix_multiply_parallel
perf stat ./matrix_multiply_parallel 2>Results/perfStats_parallel_$timestamp.txt

# Start the perf record for the matrix_mul_single, record every possible thing save the data in perf-single.data
perf record -o Results/perf-single.data -a -g -e cycles,instructions,cache-references,cache-misses,branch-instructions,branch-misses,context-switches,cpu-migrations,faults,task-clock -- ./matrix_mul_single

# Start the perf record for the matrix_multiply_parallel, record every possible thing save the data in perf-parallel.data
perf record -o Results/perf-parallel.data -a -g -e cycles,instructions,cache-references,cache-misses,branch-instructions,branch-misses,context-switches,cpu-migrations,faults,task-clock -- ./matrix_multiply_parallel

# Generate the report for the matrix_mul_single
perf report -i Results/perf-single.data --stdio >Results/perfReport_single_$timestamp.txt

# Generate the report for the matrix_multiply_parallel
perf report -i Results/perf-parallel.data --stdio >Results/perfReport_parallel_$timestamp.txt

# using gprof to generate the profile for the matrix_mul_single
gprof matrix_mul_single Results/gmon.out >Results/gprof_single_$timestamp.txt

# using gprof to generate the profile for the matrix_multiply_parallel
gprof matrix_multiply_parallel Results/gmon.out >Results/gprof_parallel_$timestamp.txt

# clean the files
make clean

# Zip the results
sudo 7z a -t7z -mx=9 -mmt=on -m0=lzma2 -md=512m -ms=on Results.7z Results/
