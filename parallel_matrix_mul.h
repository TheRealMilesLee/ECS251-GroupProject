#ifndef PARALLEL_MATRIX_MUL_H
#define PARALLEL_MATRIX_MUL_H
#include <tbb/parallel_for.h>
#include <tbb/tbb.h>

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
   * @brief Performs single-threaded matrix multiplication.
   *
   * This function multiplies two matrices (matrix1 and matrix2) and stores
   * the result in the result matrix. All matrices are represented as 2D
   * vectors of integers.
   *
   * @param matrix1 The first input matrix.
   * @param matrix2 The second input matrix.
   * @param result The matrix to store the result of the multiplication.
   * @param block_size The size of the block to be used for multiplication.
   *
   * @note The dimensions of the matrices must be compatible for
   * multiplication. Specifically, the number of columns in matrix1 must be
   * equal to the number of rows in matrix2.
   */
  void single_thread_matrix_mul(std::vector<std::vector<int>> &matrix1,
                                std::vector<std::vector<int>> &matrix2,
                                std::vector<std::vector<int>> &result,
                                size_t block_size);
  /**
   * @brief Multiplies two matrices in parallel using asynchronous tasks.
   *
   * @param matrix1 The first input matrix.
   * @param matrix2 The second input matrix.
   * @param result The resulting matrix after multiplication.
   * @param block_size The size of the block to be used for multiplication.
   */
  void parallel_computing_async(std::vector<std::vector<int>> &matrix1,
                                std::vector<std::vector<int>> &matrix2,
                                std::vector<std::vector<int>> &result,
                                size_t block_size);

  /**
   * @brief Multiplies two matrices in parallel using a FIFO (First In, First
   * Out) scheduling policy.
   *
   * @param matrix1 The first input matrix.
   * @param matrix2 The second input matrix.
   * @param result The resulting matrix after multiplication.
   * @param block_size The size of the block to be used for multiplication.
   */
  void parallel_computing_fifo(std::vector<std::vector<int>> &matrix1,
                               std::vector<std::vector<int>> &matrix2,
                               std::vector<std::vector<int>> &result,
                               size_t block_size);

  /**
   * @brief Multiplies two matrices in parallel using a LIFO (Last In, First
   * Out) scheduling policy.
   *
   * @param matrix1 The first input matrix.
   * @param matrix2 The second input matrix.
   * @param result The resulting matrix after multiplication.
   * @param block_size The size of the block to be used for multiplication.
   */
  void parallel_computing_lifo(std::vector<std::vector<int>> &matrix1,
                               std::vector<std::vector<int>> &matrix2,
                               std::vector<std::vector<int>> &result,
                               size_t block_size);

  /**
   * @brief Multiplies two matrices in parallel using Intel Threading Building
   * Blocks (TBB).
   *
   * @param matrix1 The first input matrix.
   * @param matrix2 The second input matrix.
   * @param result The resulting matrix after multiplication.
   * @param block_size The size of the block to be used for multiplication.
   */
  void parallel_computing_tbb(std::vector<std::vector<int>> &matrix1,
                              std::vector<std::vector<int>> &matrix2,
                              std::vector<std::vector<int>> &result,
                              size_t block_size);

  void parallel_computing_simple_multithread(
      std::vector<std::vector<int>> &matrix1,
      std::vector<std::vector<int>> &matrix2,
      std::vector<std::vector<int>> &result, size_t block_size);
  // Function to clear destination matrix
  void clear_matrix(vector<vector<int>> &dst);

  bool matrix_validation(vector<vector<int>> src1, vector<vector<int>> src2,
                         vector<vector<int>> incoming_dst);
};

void MatrixBenchMark::matrix_mul(vector<vector<int>> &src1,
                                 vector<vector<int>> &src2,
                                 vector<vector<int>> &dst, size_t blockSize,
                                 size_t start, size_t end)
{
  // Perform matrix multiplication for the given block range using block ik
  // method
  for (size_t iblock = start; iblock < end; iblock += blockSize)
  {
    for (size_t kblock = 0; kblock < src2.size(); kblock += blockSize)
    {
      for (size_t jblock = 0; jblock < dst.size(); jblock += blockSize)
      {
        for (size_t i = iblock; i < min(iblock + blockSize, src1.size()); i++)
        {
          for (size_t k = kblock; k < min(kblock + blockSize, src2.size());
               k++)
          {
            for (size_t j = jblock; j < min(jblock + blockSize, dst.size());
                 j++)
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

  // Calculate the total number of blocks
  size_t total_blocks = (src1.size() + blockSize - 1) / blockSize;

  // Vector to store futures
  vector<future<void>> futures;

  // Create tasks for each block
  for (size_t i = 0; i < src1.size(); i += blockSize)
  {
    // Launch async task for each block
    futures.push_back(
        async(launch::async, [this, &src1, &src2, &dst, blockSize, i]() {
          this->matrix_mul(src1, src2, dst, blockSize, i,
                           min(i + blockSize, src1.size()));
        }));

    // If we have launched as many tasks as threads, wait for some to complete
    if (futures.size() >= num_threads)
    {
      for (auto &f : futures)
      {
        f.get();
      }
      futures.clear();
    }
  }

  // Wait for remaining tasks to complete
  for (auto &f : futures)
  {
    f.get();
  }
}

void MatrixBenchMark::parallel_computing_fifo(vector<vector<int>> &src1,
                                              vector<vector<int>> &src2,
                                              vector<vector<int>> &dst,
                                              size_t blockSize)
{
}

void MatrixBenchMark::parallel_computing_lifo(vector<vector<int>> &src1,
                                              vector<vector<int>> &src2,
                                              vector<vector<int>> &dst,
                                              size_t blockSize)
{
}

void MatrixBenchMark::parallel_computing_tbb(vector<vector<int>> &src1,
                                             vector<vector<int>> &src2,
                                             vector<vector<int>> &dst,
                                             size_t blockSize)
{
  // Use TBB's parallel_for with blocked_range
  tbb::parallel_for(tbb::blocked_range<size_t>(0, src1.size(), blockSize),
                    [&](const tbb::blocked_range<size_t> &range) {
                      matrix_mul(src1, src2, dst, blockSize, range.begin(),
                                 range.end());
                    });
}

void MatrixBenchMark::parallel_computing_simple_multithread(
    std::vector<std::vector<int>> &matrix1,
    std::vector<std::vector<int>> &matrix2,
    std::vector<std::vector<int>> &result, size_t block_size)
{
  // Get the number of available threads
  size_t num_threads = thread::hardware_concurrency();

  // Vector to store threads
  vector<thread> threads;

  // Create tasks for each block
  for (size_t i = 0; i < matrix1.size(); i += block_size)
  {
    // Launch a thread for each block
    threads.push_back(
        thread([this, &matrix1, &matrix2, &result, block_size, i]() {
          this->matrix_mul(matrix1, matrix2, result, block_size, i,
                           min(i + block_size, matrix1.size()));
        }));
  }

  // Wait for all threads to complete
  for (auto &t : threads)
  {
    t.join();
  }

  // Clear the threads vector
  threads.clear();
}

void MatrixBenchMark::clear_matrix(vector<vector<int>> &dst)
{
  for (auto &row : dst)
  {
    fill(row.begin(), row.end(), 0);
  }
}

void MatrixBenchMark::single_thread_matrix_mul(vector<vector<int>> &matrix1,
                                               vector<vector<int>> &matrix2,
                                               vector<vector<int>> &result,
                                               size_t block_size)
{
  matrix_mul(matrix1, matrix2, result, block_size, 0, matrix1.size());
}

bool MatrixBenchMark::matrix_validation(vector<vector<int>> src1,
                                        vector<vector<int>> src2,
                                        vector<vector<int>> incoming_dst)
{
  vector<vector<int>> compare_matrix(src1.size(), vector<int>(src1.size()));
  for (size_t i = 0; i < src1.size(); i++)
  {
    for (size_t j = 0; j < src2[0].size(); j++)
    {
      for (size_t k = 0; k < src2.size(); k++)
      {
        compare_matrix[i][j] += src1[i][k] * src2[k][j];
      }
    }
  }

  // start the comparison
  for (size_t row = 0; row < incoming_dst.size(); row++)
  {
    for (size_t col = 0; col < incoming_dst[0].size(); col++)
    {
      if (compare_matrix[row][col] != incoming_dst[row][col])
      {
        cout << "Matrix validation failed at row: " << row
             << " and column: " << col << endl;
        return false;
      }
    }
  }
  cout << "Matrix validation passed" << endl;
  return true;
}

#endif  // PARALLEL_MATRIX_MUL_H
