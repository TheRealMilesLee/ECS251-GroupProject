#include <chrono>
#include <iostream>
#include <vector>
#include <random>

#include "parallel_matrix_mul.h"
using namespace std;

int main(int argc, char *argv[])
{
  // Default values
  size_t matrix_size = 4096;
  size_t block_size = 128;

  // Parse command line arguments
  if (argc > 1) matrix_size = static_cast<size_t>(atoi(argv[1]));
  if (argc > 2) block_size = static_cast<size_t>(atoi(argv[2]));

  MatrixBenchMark matrixBenchMark;

  // Initialize random number generator
  random_device rd;
  mt19937 gen(rd());
  uniform_int_distribution<int> dist(-100, 100);

  // Initialize matrices with random values
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

  cout << "Starting matrix multiplication benchmark...\n";
  cout << "Matrix size: " << matrix_size << "x" << matrix_size
       << ", Block size: " << block_size << "\n";
  cout << "=========================================================\n";

  // Test async implementation
  auto start_time = std::chrono::high_resolution_clock::now();
  matrixBenchMark.parallel_computing_async(src1, src2, dst, block_size);
  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
      end_time - start_time);
  double seconds = static_cast<double>(duration.count()) / 1000000.0;
  cout << "Async implementation time: " << seconds << " seconds ("
       << duration.count() << " microseconds)\n";

  // Clear dst for next testing
  for (auto &row : dst)
  {
    fill(row.begin(), row.end(), 0);
  }

  // Test FIFO implementation
  start_time = std::chrono::high_resolution_clock::now();
  matrixBenchMark.parallel_computing_fifo(src1, src2, dst, block_size);
  end_time = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(
      end_time - start_time);
  seconds = static_cast<double>(duration.count()) / 1000000.0;
  cout << "FIFO implementation time: " << seconds << " seconds ("
       << duration.count() << " microseconds)\n";

  // Clear dst for next testing
  for (auto &row : dst)
  {
    fill(row.begin(), row.end(), 0);
  }

  // Test LIFO implementation
  start_time = std::chrono::high_resolution_clock::now();
  matrixBenchMark.parallel_computing_lifo(src1, src2, dst, block_size);
  end_time = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(
      end_time - start_time);
  seconds = static_cast<double>(duration.count()) / 1000000.0;
  cout << "LIFO implementation time: " << seconds << " seconds ("
       << duration.count() << " microseconds)\n";

  // Clear dst for next testing
  for (auto &row : dst)
  {
    fill(row.begin(), row.end(), 0);
  }

  // Test PPL implementation
  start_time = std::chrono::high_resolution_clock::now();
  matrixBenchMark.parallel_computing_ppl(src1, src2, dst, block_size);
  end_time = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(
      end_time - start_time);
  seconds = static_cast<double>(duration.count()) / 1000000.0;
  cout << "PPL implementation time: " << seconds << " seconds ("
       << duration.count() << " microseconds)\n";

  // Clear dst for next testing
  for (auto &row : dst)
  {
    fill(row.begin(), row.end(), 0);
  }

  // Test TBB implementation
  start_time = std::chrono::high_resolution_clock::now();
  matrixBenchMark.parallel_computing_tbb(src1, src2, dst, block_size);
  end_time = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(
      end_time - start_time);
  seconds = static_cast<double>(duration.count()) / 1000000.0;
  cout << "TBB implementation time: " << seconds << " seconds ("
       << duration.count() << " microseconds)\n";

  cout << "=========================================================\n";
  return 0;
}
