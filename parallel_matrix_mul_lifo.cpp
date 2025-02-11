#include "parallel_matrix_mul.h"
using namespace std;

int main(int argc, char *argv[])
{
  // Default values
  size_t matrix_size = 4096;
  size_t block_size = 128;
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

  auto start_time = std::chrono::high_resolution_clock::now();
  matrixBenchMark.parallel_computing_lifo(src1, src2, dst, block_size);
  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
      end_time - start_time);

  double seconds = static_cast<double>(duration.count()) / 1000000.0;

  cout << "Matrix multiplication time with LIFO: " << seconds
       << " seconds or " << duration.count() % 1000000 << " microseconds"
       << endl;

  // Clear dst for next testing
  matrixBenchMark.clear_matrix(dst);
  return 0;
}
