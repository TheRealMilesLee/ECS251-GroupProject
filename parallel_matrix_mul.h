#ifndef PARALLEL_MATRIX_MUL_H
#define PARALLEL_MATRIX_MUL_H
#include <tbb/parallel_for.h>
#include <tbb/tbb.h>
#include <tbb/blocked_range2d.h>

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
private:
  queue<function<void()>> task_queue; // FIFO queue for tasks
  stack<function<void()>> task_stack; // LIFO stack for tasks
  mutex task_mutex; // Mutex for tasks
  condition_variable cond; // Condition variable for synchronization
  bool stop = false; // Stop flag for thread pool

public:
  /**
   * @brief This function is used to clear the matrix
   *
   * @param dst The matrix to be cleared
   */
  template<typename T>
  void clear_matrix(vector<vector<T>> &dst);

  template<typename T>
  void matrix_mul(std::vector<std::vector<T>> &matrix1,
                  std::vector<std::vector<T>> &matrix2,
                  std::vector<std::vector<T>> &result,
                  size_t block_size,
                  size_t start,
                  size_t end);
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
   * @brief This is the double version of the single_thread_matrix_mul
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
  void single_thread_matrix_mul_double(
      std::vector<std::vector<double>> &matrix1,
      std::vector<std::vector<double>> &matrix2,
      std::vector<std::vector<double>> &result,
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
   * @brief Worker func for thread pool
   */
  void worker_thread_fifo();

  /**
   * @brief Worker func for thread pool
   */
  void worker_thread_lifo();

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
  /**
   * @brief This function is a helper function for the parallel_computing_tbb
   * It does the actural matrix multiplication
   * @param range is the range of the block
   * @param src1 The first input matrix.
   * @param src2 The second input matrix.
   * @param dst The resulting matrix after multiplication.
   * @param blockSize the size of the block
   */
  void parallel_computing_tbb_helper(
      const tbb::blocked_range2d<size_t> &range,
      vector<vector<int>> &src1,
      vector<vector<int>> &src2,
      vector<vector<int>> &dst,
      size_t blockSize);

  /**
   * @brief This function is the simple multi-threaded version of the matrix
   * multiplication
   *
   * @param matrix1 The first input matrix.
   * @param matrix2 The second input matrix.
   * @param result The resulting matrix after multiplication.
   * @param block_size The size of the block to be used for multiplication.
   */
  void parallel_computing_simple_multithread(
      std::vector<std::vector<int>> &matrix1,
      std::vector<std::vector<int>> &matrix2,
      std::vector<std::vector<int>> &result,
      size_t block_size);

  bool matrix_validation(vector<vector<int>> src1,
                         vector<vector<int>> src2,
                         vector<vector<int>> incoming_dst);
};
template<typename T>
void MatrixBenchMark::matrix_mul(vector<vector<T>> &src1,
                                 vector<vector<T>> &src2,
                                 vector<vector<T>> &dst,
                                 size_t blockSize,
                                 size_t start,
                                 size_t end)
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
  // Vector to store futures
  vector<future<void>> futures;

  // Create tasks for each block
  for (size_t i = 0; i < src1.size(); i += blockSize)
  {
    // Launch async task for each block
    futures.push_back(async(
        launch::async,
        [this, &src1, &src2, &dst, blockSize, i]()
        {
          this->matrix_mul(
              src1, src2, dst, blockSize, i, min(i + blockSize, src1.size()));
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

// Worker functions for the thread pool implementations
void MatrixBenchMark::worker_thread_fifo()
{
  while (true)
  {
    function<void()> task;
    {
      unique_lock<mutex> lock(task_mutex);

      // Waiting until there is a task to
      // execute or the pool is stopped
      cond.wait(lock, [this] { return !task_queue.empty() || stop; });

      // exit the thread in case the pool
      // is stopped and there are no tasks
      if (stop && task_queue.empty())
      {
        return;
      }

      // Get the next task from the queue
      task = std::move(task_queue.front());
      task_queue.pop();
    }
    task();
  }
}

void MatrixBenchMark::worker_thread_lifo()
{
  while (true)
  {
    function<void()> task;
    {
      unique_lock<mutex> lock(task_mutex);

      // Waiting until there is a task to
      // execute or the pool is stopped
      cond.wait(lock, [this] { return !task_stack.empty() || stop; });

      // exit the thread in case the pool
      // is stopped and there are no tasks
      if (stop && task_stack.empty())
      {
        return;
      }

      // Get the next task from the queue
      task = std::move(task_stack.top());
      task_stack.pop();
    }
    task();
  }
}

void MatrixBenchMark::parallel_computing_fifo(vector<vector<int>> &src1,
                                              vector<vector<int>> &src2,
                                              vector<vector<int>> &dst,
                                              size_t block_size)
{
  // Get the number of available threads
  size_t num_threads = thread::hardware_concurrency();

  // Vector to store threads
  vector<thread> threads;

  // Create the thread pool
  for (int i = 0; i < num_threads; ++i)
  {
    threads.emplace_back([this] { worker_thread_fifo(); });
  }

  // Create tasks for each block
  for (int i = 0; i < src1.size(); i += block_size)
  {
    lock_guard<mutex> lock(task_mutex);
    task_queue.push(
        [this, &src1, &src2, &dst, block_size, i]
        {
          this->matrix_mul(src1,
                           src2,
                           dst,
                           block_size,
                           i,
                           min(i + block_size, src1.size()));
        });
  }

  {
    // Lock the queue to update the stop flag safely
    unique_lock<mutex> lock(task_mutex);
    stop = true;
  }

  // Notify all threads
  cond.notify_all();

  // Joining all worker threads to ensure they have
  // completed their tasks
  for (auto &thread : threads)
  {
    thread.join();
  }
}

void MatrixBenchMark::parallel_computing_lifo(vector<vector<int>> &src1,
                                              vector<vector<int>> &src2,
                                              vector<vector<int>> &dst,
                                              size_t block_size)
{
  // Get the number of available threads
  size_t num_threads = thread::hardware_concurrency();

  // Vector to store threads
  vector<thread> threads;

  // Create the thread pool
  for (int i = 0; i < num_threads; ++i)
  {
    threads.emplace_back([this] { worker_thread_lifo(); });
  }

  // Create tasks for each block
  for (size_t i = 0; i < src1.size(); i += block_size)
  {
    lock_guard<mutex> lock(task_mutex);
    task_stack.push(
        [this, &src1, &src2, &dst, block_size, i]
        {
          this->matrix_mul(src1,
                           src2,
                           dst,
                           block_size,
                           i,
                           min(i + block_size, src1.size()));
        });
  }

  {
    // Lock the queue to update the stop flag safely
    unique_lock<mutex> lock(task_mutex);
    stop = true;
  }

  // Notify all threads
  cond.notify_all();

  // Joining all worker threads to ensure they have
  // completed their tasks
  for (auto &thread : threads)
  {
    thread.join();
  }
}

void MatrixBenchMark::parallel_computing_tbb_helper(
    const tbb::blocked_range2d<size_t> &range,
    vector<vector<int>> &src1,
    vector<vector<int>> &src2,
    vector<vector<int>> &dst,
    size_t blockSize)
{
  for (size_t i = range.rows().begin(); i < range.rows().end();
       i += blockSize)
  {
    for (size_t j = range.cols().begin(); j < range.cols().end();
         j += blockSize)
    {
      matrix_mul(src1, src2, dst, blockSize, i, j);
    }
  }
}

void MatrixBenchMark::parallel_computing_tbb(vector<vector<int>> &src1,
                                             vector<vector<int>> &src2,
                                             vector<vector<int>> &dst,
                                             size_t blockSize)
{
  /**
    In this implementation, we divide the matrix into non-overlapping blocks and assign each block to a separate thread for parallel computation.

    Example: Suppose `src1`, `src2`, and `dst` are 4x4 matrices:

    src1:                   src2:
    -----------------        ---------------------
    |  1 |  2 |  3 |  4 |   | 17 | 18 | 19 | 20 |
    -----------------        ---------------------
    |  5 |  6 |  7 |  8 |   | 21 | 22 | 23 | 24 |
    -----------------        ---------------------
    |  9 | 10 | 11 | 12 |   | 25 | 26 | 27 | 28 |
    -----------------        ---------------------
    | 13 | 14 | 15 | 16 |   | 29 | 30 | 31 | 32 |
    -----------------        ---------------------

    If the block size is 2x2, we partition the matrices into 2x2 submatrices.
    Each thread is assigned a specific (i, j) block from `src1` and `src2` to compute a corresponding block in `dst`.

    Example: The first block (top-left) multiplication would be:

    Block from `src1`:      Block from `src2`:      Resulting block in `dst`:
    ---------               -----------             ---------------------------------------------------------
    |  1 |  2 |            | 17 | 18 |            | 1*17 + 2*21 + 3*25 + 4*29 | 1*18 + 2*22 + 3*26 + 4*30 |
    ---------  x           -----------  =         ---------------------------------------------------------
    |  5 |  6 |            | 21 | 22 |            | 5*17 + 6*21 + 7*25 + 8*29 | 5*18 + 6*22 + 7*26 + 8*30 |
    ---------               -----------             ---------------------------------------------------------

    The same process is executed in parallel for each 2x2 block by different threads.

    Since each block computation is independent of the others, we use Intel Threading Building Blocks (TBB)
    to parallelize the computation, ensuring efficient utilization of multi-core processors.
 */

  size_t N = src1.size();
  tbb::parallel_for(
      tbb::blocked_range2d<size_t>(0, N, blockSize, 0, N, blockSize),
      [&](const tbb::blocked_range2d<size_t> &range)
      { parallel_computing_tbb_helper(range, src1, src2, dst, blockSize); });
}

void MatrixBenchMark::parallel_computing_simple_multithread(
    std::vector<std::vector<int>> &matrix1,
    std::vector<std::vector<int>> &matrix2,
    std::vector<std::vector<int>> &result,
    size_t block_size)
{
  // Get the number of available threads
  size_t num_threads = thread::hardware_concurrency();

  // Vector to store threads
  vector<thread> threads;

  // Create tasks for each block
  for (size_t i = 0; i < matrix1.size(); i += block_size)
  {
    // Launch a thread for each block
    threads.push_back(thread(
        [this, &matrix1, &matrix2, &result, block_size, i]()
        {
          this->matrix_mul(matrix1,
                           matrix2,
                           result,
                           block_size,
                           i,
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

template<typename T>
void MatrixBenchMark::clear_matrix(vector<vector<T>> &dst)
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

void MatrixBenchMark::single_thread_matrix_mul_double(
    vector<vector<double>> &matrix1,
    vector<vector<double>> &matrix2,
    vector<vector<double>> &result,
    size_t block_size)
{
  matrix_mul(matrix1, matrix2, result, block_size, 0, matrix1.size());
}

#endif // PARALLEL_MATRIX_MUL_H
