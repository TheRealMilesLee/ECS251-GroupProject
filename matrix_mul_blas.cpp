#include "parallel_matrix_mul.h"
#include <cblas.h>

using namespace std;
int main()
{
  // Default values
  size_t n = 4096;

  // Initialize matrices with size n x n
  std::vector<double> src1(n * n);
  std::vector<double> src2(n * n);
  std::vector<double> dst(n * n, 0.0);

  // Fill mat 1
  for (size_t row = 0; row < n; row++)
  {
    for (size_t col = 0; col < n; col++)
    {
      src1[row * n + col] = static_cast<double>(row) + static_cast<double>(col);
      src2[row * n + col] = static_cast<double>(row) + static_cast<double>(col);
    }
  }
  
//   // Fill mat2
//   for (int i = 0; i < n; ++i) {
//     for (int j = 0; j < n; ++j) {
//       src2[i * n + j] = rand();
//     }
//   }

  auto start_time = std::chrono::high_resolution_clock::now();

  // Perform matrix multiplication using cblas_dgemm
  cblas_dgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans,
              n, n, n,
              1.0, src1.data(), n,
              src2.data(), n,
              0.0, dst.data(), n);

  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
      end_time - start_time);

  double seconds = static_cast<double>(duration.count()) / 1000000.0;

  cout << "Matrix multiplication time with BLAS: " << seconds
       << " seconds or " << duration.count() % 1000000 << " microseconds"
       << endl;

  FILE *f = fopen("matrix_mul_blas.txt", "w");
  for (size_t i = 0; i < n; i++)
  {
    for (size_t j = 0; j < n; j++)
    {
      fprintf(f, "%lf ", dst[i * n + j]);
    }
    fprintf(f, "\n");
  }
  fclose(f);

  return 0;
}
