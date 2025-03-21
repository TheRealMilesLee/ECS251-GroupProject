#include "parallel_matrix_mul.h"
using namespace std;

int main(int argc, char **argv)
{
  // Default values
  if (argc < 2) {
    cerr << "Usage: " << argv[0] << " <matrix_size>" << endl;
    return 1;
  }
  size_t matrix_size = static_cast<size_t>(atoi(argv[1]));
  size_t block_size = 128;
  MatrixBenchMark matrixBenchMark;

  // Initialize matrices
  vector<vector<int>> src1(matrix_size, vector<int>(matrix_size));
  vector<vector<int>> src2(matrix_size, vector<int>(matrix_size));
  vector<vector<int>> dst(matrix_size, vector<int>(matrix_size, 0));

  for (size_t row = 0; row < matrix_size; row++)
  {
    for (size_t col = 0; col < matrix_size; col++)
    {
      src1[row][col] = static_cast<int>(row) + static_cast<int>(col);
      src2[row][col] = static_cast<int>(row) + static_cast<int>(col);
    }
  }

  auto start_time = std::chrono::high_resolution_clock::now();
  matrixBenchMark.parallel_computing_async(src1, src2, dst, block_size);
  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
      end_time - start_time);

  double seconds = static_cast<double>(duration.count()) / 1000000.0;

  cout << "Matrix multiplication time with async: " << seconds
       << " seconds or " << duration.count() % 1000000 << " microseconds"
       << endl;
  FILE *f = fopen("parallel_matrix_mul_async.txt", "w");
  for (size_t i = 0; i < dst.size(); i++)
  {
    for (size_t j = 0; j < dst[0].size(); j++)
    {
      fprintf(f, "%d ", dst[i][j]);
    }
    fprintf(f, "\n");
  }
  fclose(f);
  matrixBenchMark.clear_matrix(dst);

  return 0;
}
