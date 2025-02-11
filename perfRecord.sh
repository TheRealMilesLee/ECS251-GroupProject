#!/bin/bash

make clean
make

timestamp=$(date +"%Y%m%d%H%M%S")

# Start the perf stat for the matrix_mul_single
perf stat ./matrix_mul_single 2>Results/perfStats_single_$timestamp.txt

# Start the perf stat for the matrix_multiply_parallel
perf stat ./matrix_multiply_parallel 2>Results/perfStats_parallel_$timestamp.txt

# Start the perf stat for the matrix_mul_async
perf stat ./matrix_mul_async 2>Results/perfStats_async_$timestamp.txt

# Start the perf stat for the matrix_mul_fifo
perf stat ./matrix_mul_fifo 2>Results/perfStats_fifo_$timestamp.txt

# Start the perf stat for the matrix_mul_lifo
perf stat ./matrix_mul_lifo 2>Results/perfStats_lifo_$timestamp.txt

# Start the perf stat for the matrix_mul_tbb
perf stat ./matrix_mul_tbb 2>Results/perfStats_tbb_$timestamp.txt

# Start the perf stat for the matrix_mul_ppl
perf stat ./matrix_mul_ppl 2>Results/perfStats_ppl_$timestamp.txt

# Start the perf record for the matrix_mul_single, record every possible thing save the data in perf-single.data
perf record -o Results/perf-single.data -a -g -e cycles,instructions,cache-references,cache-misses,branch-instructions,branch-misses,context-switches,cpu-migrations,faults,task-clock -- ./matrix_mul_single

# Start the perf record for the matrix_multiply_parallel, record every possible thing save the data in perf-parallel.data
perf record -o Results/perf-parallel.data -a -g -e cycles,instructions,cache-references,cache-misses,branch-instructions,branch-misses,context-switches,cpu-migrations,faults,task-clock -- ./matrix_multiply_parallel

# Stat the perf record for the matrix_mul_async, record every possible thing save the data in perf-async.data
perf record -o Results/perf-async.data -a -g -e cycles,instructions,cache-references,cache-misses,branch-instructions,branch-misses,context-switches,cpu-migrations,faults,task-clock -- ./matrix_mul_async

# Stat the perf record for the matrix_mul_fifo, record every possible thing save the data in perf-fifo.data
perf record -o Results/perf-fifo.data -a -g -e cycles,instructions,cache-references,cache-misses,branch-instructions,branch-misses,context-switches,cpu-migrations,faults,task-clock -- ./matrix_mul_fifo

# Stat the perf record for the matrix_mul_lifo, record every possible thing save the data in perf-lifo.data
perf record -o Results/perf-lifo.data -a -g -e cycles,instructions,cache-references,cache-misses,branch-instructions,branch-misses,context-switches,cpu-migrations,faults,task-clock -- ./matrix_mul_lifo

# Stat the perf record for the matrix_mul_tbb, record every possible thing save the data in perf-tbb.data
perf record -o Results/perf-tbb.data -a -g -e cycles,instructions,cache-references,cache-misses,branch-instructions,branch-misses,context-switches,cpu-migrations,faults,task-clock -- ./matrix_mul_tbb

# Stat the perf record for the matrix_mul_ppl, record every possible thing save the data in perf-ppl.data
perf record -o Results/perf-ppl.data -a -g -e cycles,instructions,cache-references,cache-misses,branch-instructions,branch-misses,context-switches,cpu-migrations,faults,task-clock -- ./matrix_mul_ppl

# Generate the report for the matrix_mul_async
perf report -i Results/perf-async.data --stdio >Results/perfReport_async_$timestamp.txt

# Generate the report for the matrix_mul_fifo
perf report -i Results/perf-fifo.data --stdio >Results/perfReport_fifo_$timestamp.txt

# Generate the report for the matrix_mul_lifo
perf report -i Results/perf-lifo.data --stdio >Results/perfReport_lifo_$timestamp.txt

# Generate the report for the matrix_mul_tbb
perf report -i Results/perf-tbb.data --stdio >Results/perfReport_tbb_$timestamp.txt

# Generate the report for the matrix_mul_ppl
perf report -i Results/perf-ppl.data --stdio >Results/perfReport_ppl_$timestamp.txt

# Generate the report for the matrix_mul_single
perf report -i Results/perf-single.data --stdio >Results/perfReport_single_$timestamp.txt

# Generate the report for the matrix_multiply_parallel
perf report -i Results/perf-parallel.data --stdio >Results/perfReport_parallel_$timestamp.txt

# clean the files
make clean

# Zip the results
sudo 7z a -t7z -mx=9 -mmt=on -m0=lzma2 -md=1024m -ms=on Results.7z Results/
