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

# Start validate the results
echo "Validating the results"
diff -u matrix_mul_single.txt parallel_matrix_mul_async.txt | grep -E "^\+" | grep -v "+++" >Results/diff_parallel_async_$timestamp.txt
diff -u matrix_mul_single.txt parallel_matrix_mul_fifo.txt | grep -E "^\+" | grep -v "+++" >Results/diff_parallel_fifo_$timestamp.txt
diff -u matrix_mul_single.txt parallel_matrix_mul_lifo.txt | grep -E "^\+" | grep -v "+++" >Results/diff_parallel_lifo_$timestamp.txt
diff -u matrix_mul_single.txt parallel_matrix_mul_tbb.txt | grep -E "^\+" | grep -v "+++" >Results/diff_parallel_tbb_$timestamp.txt
diff -u matrix_mul_single.txt parallel_matrix_mul_standard.txt | grep -E "^\+" | grep -v "+++" >Results/diff_parallel_standard_$timestamp.txt

# Check file size, if size is 0 then the files are same, means the test is passed
pushd Results/
# Using the wc command to get the file size for diff_parallel files only
for file in diff_parallel_*.txt; do
  if [ $(wc -c <$file) -eq 0 ]; then
    echo -e "\e[32m$file is empty, Validation Passed\e[0m"
    rm -rf $file
  else
    echo -e "\e[31m$file is not empty, Validation Failed\e[0m"
    rm -rf $file
  fi
done
popd
# clean the files
make clean
rm -rf matrix_mul_single.txt
rm -rf parallel_matrix_mul_async.txt
rm -rf parallel_matrix_mul_fifo.txt
rm -rf parallel_matrix_mul_lifo.txt
rm -rf parallel_matrix_mul_tbb.txt
rm -rf parallel_matrix_mul_standard.txt

# Zip the results
sudo 7z a -t7z -mx=9 -mmt=on -m0=lzma2 -md=1024m -ms=on Results.7z Results/
