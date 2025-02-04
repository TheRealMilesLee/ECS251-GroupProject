#!/bin/bash

make clean
make

timestamp=$(date +"%Y%m%d%H%M%S")

# Start the perf stat for the matrix_mul_single
perf stat ./matrix_mul_single >perfStats_single_$timestamp.txt

# Start the perf stat for the matrix_multiply_parallel
perf stat ./matrix_multiply_parallel >perfStats_parallel_$timestamp.txt

# Start the perf record for the matrix_mul_single, record every possible thing save the data in perf-single.data
perf record -o perf-single.data -a -g -e cycles,instructions,cache-references,cache-misses,branch-instructions,branch-misses,context-switches,cpu-migrations,faults,task-clock -- ./matrix_mul_single

# Start the perf record for the matrix_multiply_parallel, record every possible thing save the data in perf-parallel.data
perf record -o perf-parallel.data -a -g -e cycles,instructions,cache-references,cache-misses,branch-instructions,branch-misses,context-switches,cpu-migrations,faults,task-clock -- ./matrix_multiply_parallel

# Generate the report for the matrix_mul_single
perf report -i perf-single.data --stdio >perfReport_single_$timestamp.txt

# Generate the report for the matrix_multiply_parallel
perf report -i perf-parallel.data --stdio >perfReport_parallel_$timestamp.txt

make clean
