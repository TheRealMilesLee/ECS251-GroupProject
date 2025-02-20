#include "parallel_matrix_mul.h"
using namespace std;
int main()
{
  // Default values
  size_t matrix_size = 8192;
  size_t block_size = 128;
  MatrixBenchMark matrixBenchMark;

  // Initialize matrices
  vector<vector<double>> src1_double(matrix_size,
                                     vector<double>(matrix_size));
  vector<vector<double>> src2_double(matrix_size,
                                     vector<double>(matrix_size));
  vector<vector<double>> dst_double(matrix_size,
                                    vector<double>(matrix_size, 0));

  for (size_t row = 0; row < matrix_size; row++)
  {
    for (size_t col = 0; col < matrix_size; col++)
    {
      src1_double[row][col] = static_cast<int>(row) + static_cast<int>(col);
      src2_double[row][col] = static_cast<int>(row) + static_cast<int>(col);
    }
  }
  auto start_time_double = std::chrono::high_resolution_clock::now();
  matrixBenchMark.single_thread_matrix_mul_double(src1_double, src2_double,
                                                  dst_double, block_size);
  auto end_time_double = std::chrono::high_resolution_clock::now();
  auto duration_double =
      std::chrono::duration_cast<std::chrono::microseconds>(
          end_time_double - start_time_double);

  double seconds_double =
      static_cast<double>(duration_double.count()) / 1000000.0;

  cout << "Matrix multiplication time with single thread double: "
       << seconds_double << " seconds or "
       << duration_double.count() % 1000000 << " microseconds" << endl;

  FILE *f_double = fopen("matrix_mul_double.txt", "w");
  for (size_t i = 0; i < matrix_size; i++)
  {
    for (size_t j = 0; j < matrix_size; j++)
    {
      fprintf(f_double, "%lf ", dst_double[i][j]);
    }
    fprintf(f_double, "\n");
  }
  fclose(f_double);
  matrixBenchMark.clear_matrix(dst_double);
}
