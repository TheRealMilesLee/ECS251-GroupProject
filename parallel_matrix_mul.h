#ifndef PARALLEL_MATRIX_MUL_H
#define PARALLEL_MATRIX_MUL_H
#include <future>
#include <thread>
#include <vector>
using namespace std;

class MatrixBenchMark
{
private:
  vector<vector<int>> src1;
  vector<vector<int>> src2;
  vector<vector<int>> dst;
  size_t blockSize;

public:
  void matrix_mul(vector<vector<int>> &src1, vector<vector<int>> &src2,
                  vector<vector<int>> &dst, size_t blockSize, size_t start,
                  size_t end);

  /**
   * @brief Performs parallel matrix multiplication asynchronously.
   *
   * This function multiplies two matrices (src1 and src2) and stores the
   * result in the destination matrix (dst). The multiplication is performed
   * in parallel using asynchronous tasks, with the computation divided into
   * blocks of size blockSize.
   *
   * @param src1 The first source matrix.
   * @param src2 The second source matrix.
   * @param dst The destination matrix to store the result of the
   * multiplication.
   * @param blockSize The size of the blocks to divide the computation into
   * for parallel processing.
   */
  void parallel_computing_async(vector<vector<int>> &src1,
                                vector<vector<int>> &src2,
                                vector<vector<int>> &dst, size_t blockSize);

  // 预留其他多线程实现的函数声明
  void parallel_computing_fifo(vector<vector<int>> &src1,
                               vector<vector<int>> &src2,
                               vector<vector<int>> &dst, size_t blockSize);

  void parallel_computing_lifo(vector<vector<int>> &src1,
                               vector<vector<int>> &src2,
                               vector<vector<int>> &dst, size_t blockSize);

  void parallel_computing_ppl(vector<vector<int>> &src1,
                              vector<vector<int>> &src2,
                              vector<vector<int>> &dst, size_t blockSize);

  void parallel_computing_tbb(vector<vector<int>> &src1,
                              vector<vector<int>> &src2,
                              vector<vector<int>> &dst, size_t blockSize);
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

void MatrixBenchMark::parallel_computing_async(vector<vector<int>> &src1,
                                               vector<vector<int>> &src2,
                                               vector<vector<int>> &dst,
                                               size_t blockSize)
{
  // Get the number of available threads
  size_t num_threads = thread::hardware_concurrency();
}

void MatrixBenchMark::parallel_computing_fifo(vector<vector<int>> &src1,
                                              vector<vector<int>> &src2,
                                              vector<vector<int>> &dst,
                                              size_t blockSize)
{
  // Get the number of available threads
  size_t num_threads = thread::hardware_concurrency();
}

void MatrixBenchMark::parallel_computing_lifo(vector<vector<int>> &src1,
                                              vector<vector<int>> &src2,
                                              vector<vector<int>> &dst,
                                              size_t blockSize)
{
  // Get the number of available threads
  size_t num_threads = thread::hardware_concurrency();
}

void MatrixBenchMark::parallel_computing_ppl(vector<vector<int>> &src1,
                                             vector<vector<int>> &src2,
                                             vector<vector<int>> &dst,
                                             size_t blockSize)
{
  // Get the number of available threads
  size_t num_threads = thread::hardware_concurrency();
}

void MatrixBenchMark::parallel_computing_tbb(vector<vector<int>> &src1,
                                             vector<vector<int>> &src2,
                                             vector<vector<int>> &dst,
                                             size_t blockSize)
{
  // Get the number of available threads
  size_t num_threads = thread::hardware_concurrency();
}

#endif // PARALLEL_MATRIX_MUL_H
