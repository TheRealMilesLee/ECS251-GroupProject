#ifndef PARALLEL_MATRIX_MUL_H
#define PARALLEL_MATRIX_MUL_H

#include <ppl.h>
#include <chrono>
#include <future>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <queue>
#include <random>
#include <stack>
#include <thread>
#include <vector>

using namespace std;

class MatrixBenchMark
{
 public:
  /**
   * @brief Multiplies two matrices using a specified block size and range.
   *
   * @param matrix1 The first input matrix.
   * @param matrix2 The second input matrix.
   * @param result The resulting matrix after multiplication.
   * @param block_size The size of the block to be used for multiplication.
   * @param start The starting index for the multiplication.
   * @param end The ending index for the multiplication.
   */
  void matrix_mul(std::vector<std::vector<int>> &matrix1,
                  std::vector<std::vector<int>> &matrix2,
                  std::vector<std::vector<int>> &result, size_t block_size,
                  size_t start, size_t end);

  /**
   * @brief Multiplies two matrices in parallel using the Parallel Patterns
   * Library (PPL).
   *
   * @param matrix1 The first input matrix.
   * @param matrix2 The second input matrix.
   * @param result The resulting matrix after multiplication.
   * @param block_size The size of the block to be used for multiplication.
   */
  void parallel_computing_ppl(std::vector<std::vector<int>> &matrix1,
                              std::vector<std::vector<int>> &matrix2,
                              std::vector<std::vector<int>> &result,
                              size_t block_size);
  // Function to clear destination matrix
  void clear_matrix(vector<vector<int>> &dst);
};

void MatrixBenchMark::matrix_mul(vector<vector<int>> &src1,
                                 vector<vector<int>> &src2,
                                 vector<vector<int>> &dst, size_t blockSize,
                                 size_t start, size_t end)
{
  // Perform matrix multiplication for the given block range
  for (size_t iblock = start; iblock < end; iblock += blockSize)
  {
    for (size_t jblock = 0; jblock < src2.size(); jblock += blockSize)
    {
      for (size_t kblock = 0; kblock < dst.size(); kblock += blockSize)
      {
        for (size_t i = iblock; i < min(iblock + blockSize, src1.size()); i++)
        {
          for (size_t j = jblock; j < min(jblock + blockSize, src2.size());
               j++)
          {
            for (size_t k = kblock; k < min(kblock + blockSize, dst.size());
                 k++)
            {
              dst[i][j] += src1[i][k] * src2[k][j];
            }
          }
        }
      }
    }
  }
}



void MatrixBenchMark::parallel_computing_ppl(vector<vector<int>> &src1,
  vector<vector<int>> &src2,
  vector<vector<int>> &dst,
  size_t blockSize)
{
}



void MatrixBenchMark::clear_matrix(vector<vector<int>> &dst)
{
  for (auto &row : dst)
  {
    fill(row.begin(), row.end(), 0);
  }
}
#endif  // PARALLEL_MATRIX_MUL_H
