#!/bin/bash
rm -rf Results/
rm -rf Results.7z
make clean

mkdir Results
make

timestamp=$(date +"%Y%m%d%H%M%S")

# Start the perf stat for the matrix_mul_single
perf stat ./matrix_mul_single 2>Results/perfStats_single_$timestamp.txt

# Start the perf stat for the parallel_matrix_mul_standard
perf stat ./parallel_matrix_mul_standard 2>Results/perfStats_parallel_standard_$timestamp.txt

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

# clean the files
make clean

# Zip the results
sudo 7z a -t7z -mx=9 -mmt=on -m0=lzma2 -md=1024m -ms=on Results.7z Results/
