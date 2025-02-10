#include <chrono>
#include <iostream>
#include <vector>

#include "parallel_matrix_mul.h"
using namespace std;

int main() {
  MatrixBenchMark matrixBenchMark;
  const int block_size = 128;
  vector<vector<int>> src1(4096, vector<int>(4096, 0));
  vector<vector<int>> src2(4096, vector<int>(4096, 0));
  vector<vector<int>> dst(4096, vector<int>(4096, 0));

  auto start_time = std::chrono::high_resolution_clock::now();
  matrixBenchMark.parallel_computing_async(src1, src2, dst, block_size);
  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
      end_time - start_time);

  double seconds = static_cast<double>(duration.count()) / 1000000.0;

  cout << "Matrix multiplication time with async: " << seconds << " seconds or "
       << duration.count() % 1000000 << " microseconds" << endl;

  // Clear dst for next testing
  for (auto &row : dst) {
    fill(row.begin(), row.end(), 0);
  }

  start_time = std::chrono::high_resolution_clock::now();
  matrixBenchMark.parallel_computing_fifo(src1, src2, dst, block_size);
  end_time = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time -
                                                                   start_time);

  seconds = static_cast<double>(duration.count()) / 1000000.0;

  cout << "Matrix multiplication time with FIFO: " << seconds << " seconds or "
       << duration.count() % 1000000 << " microseconds" << endl;

  // Clear dst for next testing
  for (auto &row : dst) {
    fill(row.begin(), row.end(), 0);
  }

  start_time = std::chrono::high_resolution_clock::now();
  matrixBenchMark.parallel_computing_lifo(src1, src2, dst, block_size);
  end_time = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time -
                                                                   start_time);

  seconds = static_cast<double>(duration.count()) / 1000000.0;

  cout << "Matrix multiplication time with LIFO: " << seconds << " seconds or "
       << duration.count() % 1000000 << " microseconds" << endl;

  // Clear dst for next testing
  for (auto &row : dst) {
    fill(row.begin(), row.end(), 0);
  }

  start_time = std::chrono::high_resolution_clock::now();
  matrixBenchMark.parallel_computing_ppl(src1, src2, dst, block_size);
  end_time = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time -
                                                                   start_time);

  seconds = static_cast<double>(duration.count()) / 1000000.0;

  cout << "Matrix multiplication time with PPL: " << seconds << " seconds or "
       << duration.count() % 1000000 << " microseconds" << endl;

  // Clear dst for next testing
  for (auto &row : dst) {
    fill(row.begin(), row.end(), 0);
  }

  start_time = std::chrono::high_resolution_clock::now();
  matrixBenchMark.parallel_computing_tbb(src1, src2, dst, block_size);
  end_time = std::chrono::high_resolution_clock::now();
  duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time -
                                                                   start_time);

  seconds = static_cast<double>(duration.count()) / 1000000.0;

  cout << "Matrix multiplication time with TBB: " << seconds << " seconds or "
       << duration.count() % 1000000 << " microseconds" << endl;
}
