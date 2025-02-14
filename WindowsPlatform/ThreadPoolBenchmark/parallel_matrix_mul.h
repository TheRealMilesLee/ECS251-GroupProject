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
  /**
    Here we want to slice the matrix into non-intercept blocks, then assign each block to a thread to compute the multiplication.
    So for example, let's say src1, src2, and dst are 4x4 matrices,like this:
    -----------------        ---------------------
    | 1 | 2 | 3 | 4 |        | 17 | 18 | 19 | 20 |
    -----------------        ---------------------
    | 5 | 6 | 7 | 8 |        | 21 | 22 | 23 | 24 |
    -----------------        ---------------------
    | 9 | 10| 11| 12|        | 25 | 26 | 27 | 28 |
    -----------------        ---------------------
    | 13| 14| 15| 16|        | 29 | 30 | 31 | 32 |
    -----------------        ---------------------

    Let's say the block size is 2x2, so the first block matrix multiplication would looks like this:
    ---------     -----------     ---------------------------------------------------------
    | 1 | 2 |     | 17 | 18 |     | 1*17 + 2*21 + 3*25 + 4*29 | 1*18 + 2*22 + 3*26 + 4*30 |
    ---------  x  -----------  =  ---------------------------------------------------------
    | 5 | 6 |     | 21 | 22 |     | 5*17 + 6*21 + 7*25 + 8*29 | 5*18 + 6*22 + 7*26 + 8*30 |
    ---------     -----------     ---------------------------------------------------------

    This whole process was being done by thread 0, and the same process is done by thread 1, thread 2, and thread 3 and so on.
    For each block, a thread is assigned to compute the multiplication.Since the execution of each block is independent of the other,
    we can parallelize the computation of each block using the Parallel Patterns Library (PPL).
  */


  const size_t num_blocks = src1.size() / blockSize; // Get the number of blocks

  concurrency::parallel_for(size_t(0), num_blocks, [&](size_t i)
  {
    for (size_t j = 0; j < num_blocks; ++j)
    {
      vector<vector<int>> src1_block(blockSize, vector<int>(blockSize));
      vector<vector<int>> src2_block(blockSize, vector<int>(blockSize));
      vector<vector<int>> dst_block(blockSize, vector<int>(blockSize, 0));

      // Get the sub-matrix block for src1 and src2
      for (size_t bi = 0; bi < blockSize; ++bi)
      {
        for (size_t bj = 0; bj < blockSize; ++bj)
        {
          src1_block[bi][bj] = src1[i * blockSize + bi][j * blockSize + bj];
          src2_block[bi][bj] = src2[i * blockSize + bi][j * blockSize + bj];
        }
      }

      // Matrix Multiplication
      matrix_mul(src1_block, src2_block, dst_block, blockSize, 0, blockSize);

      // Write back the result to the destination matrix
      for (size_t bi = 0; bi < blockSize; ++bi)
      {
        for (size_t bj = 0; bj < blockSize; ++bj)
        {
          dst[i * blockSize + bi][j * blockSize + bj] = dst_block[bi][bj];
        }
      }
    }
  });
}


void MatrixBenchMark::clear_matrix(vector<vector<int>> &dst)
{
  for (auto &row : dst)
  {
    fill(row.begin(), row.end(), 0);
  }
}
#endif  // PARALLEL_MATRIX_MUL_H
