#include <chrono>
#include <functional>
#include <iomanip>
#include <iostream>
#include <random>
#include <vector>

#include "parallel_matrix_mul.h"
using namespace std;

// Function to clear destination matrix
static void clear_matrix(vector<vector<int>> &dst)
{
  for (auto &row : dst)
  {
    fill(row.begin(), row.end(), 0);
  }
}

// Function to run and time a matrix multiplication method
double run_benchmark(
    const string &method_name,
    function<void(vector<vector<int>> &, vector<vector<int>> &,
                  vector<vector<int>> &, size_t)>
        method,
    MatrixBenchMark &benchmark, vector<vector<int>> &src1,
    vector<vector<int>> &src2, vector<vector<int>> &dst, size_t block_size)
{
  auto start_time = chrono::high_resolution_clock::now();
  method(src1, src2, dst, block_size);
  auto end_time = chrono::high_resolution_clock::now();
  auto duration =
      chrono::duration_cast<chrono::microseconds>(end_time - start_time);
  double seconds = static_cast<double>(duration.count()) / 1000000.0;

  cout << left << setw(25) << method_name << ": " << fixed << setprecision(6)
       << seconds << " seconds (" << duration.count() << " microseconds)"
       << endl;

  clear_matrix(dst);
  return seconds;
}

int main(int argc, char *argv[])
{
  // Default values
  size_t matrix_size = 512;  // Increased from 256
  size_t block_size = 32;    // Increased from 16

  // Parse command line arguments
  if (argc > 1)
  {
    matrix_size = static_cast<size_t>(atoi(argv[1]));
  }
  if (argc > 2)
  {
    block_size = static_cast<size_t>(atoi(argv[2]));
  }

  MatrixBenchMark matrixBenchMark;

  // Initialize random number generator
  random_device rd;
  mt19937 gen(rd());
  uniform_int_distribution<int> dist(-100, 100);

  // Initialize matrices
  vector<vector<int>> src1(matrix_size, vector<int>(matrix_size));
  vector<vector<int>> src2(matrix_size, vector<int>(matrix_size));
  vector<vector<int>> dst(matrix_size, vector<int>(matrix_size, 0));

  // Fill matrices with random values
  for (auto &row : src1)
  {
    for (int &val : row)
    {
      val = dist(gen);
    }
  }
  for (auto &row : src2)
  {
    for (int &val : row)
    {
      val = dist(gen);
    }
  }

  cout << "\nMatrix Multiplication Benchmark" << endl;
  cout << string(80, '=') << endl;
  cout << "Matrix size: " << matrix_size << "x" << matrix_size
       << ", Block size: " << block_size << endl;
  cout << string(80, '-') << endl;

  // Vector to store timing results
  vector<pair<string, double>> results;

  // Run benchmarks for each implementation
  results.push_back(
      {"Single Thread", run_benchmark(
                            "Single Thread",
                            [&](auto &s1, auto &s2, auto &d, auto b) {
                              matrixBenchMark.single_thread_computing(s1, s2,
                                                                      d, b);
                            },
                            matrixBenchMark, src1, src2, dst, block_size)});

  results.push_back(
      {"Async", run_benchmark(
                    "Async",
                    [&](auto &s1, auto &s2, auto &d, auto b) {
                      matrixBenchMark.parallel_computing_async(s1, s2, d, b);
                    },
                    matrixBenchMark, src1, src2, dst, block_size)});

  results.push_back(
      {"FIFO", run_benchmark(
                   "FIFO",
                   [&](auto &s1, auto &s2, auto &d, auto b) {
                     matrixBenchMark.parallel_computing_fifo(s1, s2, d, b);
                   },
                   matrixBenchMark, src1, src2, dst, block_size)});

  results.push_back(
      {"LIFO", run_benchmark(
                   "LIFO",
                   [&](auto &s1, auto &s2, auto &d, auto b) {
                     matrixBenchMark.parallel_computing_lifo(s1, s2, d, b);
                   },
                   matrixBenchMark, src1, src2, dst, block_size)});

  results.push_back(
      {"PPL", run_benchmark(
                  "PPL",
                  [&](auto &s1, auto &s2, auto &d, auto b) {
                    matrixBenchMark.parallel_computing_ppl(s1, s2, d, b);
                  },
                  matrixBenchMark, src1, src2, dst, block_size)});

  results.push_back(
      {"TBB", run_benchmark(
                  "TBB",
                  [&](auto &s1, auto &s2, auto &d, auto b) {
                    matrixBenchMark.parallel_computing_tbb(s1, s2, d, b);
                  },
                  matrixBenchMark, src1, src2, dst, block_size)});

  // Find the fastest implementation
  auto fastest = min_element(
      results.begin(), results.end(),
      [](const auto &a, const auto &b) { return a.second < b.second; });

  cout << string(80, '-') << endl;
  cout << "Fastest implementation: " << fastest->first << " (" << fixed
       << setprecision(6) << fastest->second << " seconds)" << endl;
  cout << string(80, '=') << endl;

  return 0;
}
