#ifndef PARALLEL_MATRIX_MUL_H
#define PARALLEL_MATRIX_MUL_H

#include <cblas.h>
#include <omp.h>
#include <tbb/blocked_range2d.h>
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

using std::async;
using std::condition_variable;
using std::cout;
using std::endl;
using std::fill;
using std::function;
using std::future;
using std::launch;
using std::lock_guard;
using std::min;
using std::mutex;
using std::queue;
using std::stack;
using std::thread;
using std::unique_lock;
using std::unique_ptr;
using std::vector;

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
   * @brief THis function is the openmp version of the matrix multiplication
   *
   * @param src1 is the first matrix
   * @param src2 is the second matrix
   * @param dst is the resulting matrix
   * @param blockSize is the size of the block
   */
  void parallel_computing_openmp(std::vector<std::vector<int>> &src1,
                                 std::vector<std::vector<int>> &src2,
                                 std::vector<std::vector<int>> &dst,
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
  for (size_t i = 0; i < num_threads; ++i)
  {
    threads.emplace_back([this] { worker_thread_fifo(); });
  }

  // Create tasks for each block
  for (size_t i = 0; i < src1.size(); i += block_size)
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
  for (size_t i = 0; i < num_threads; ++i)
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

void MatrixBenchMark::parallel_computing_tbb(vector<vector<int>> &src1,
                                             vector<vector<int>> &src2,
                                             vector<vector<int>> &dst,
                                             size_t blockSize)
{
  parallel_for(
      blocked_range<size_t>(start, end, blockSize),
      [&](const blocked_range<size_t> &iblock_range)
      {
        vector<vector<double>> local_dst(
            dst.size(), vector<double>(dst[0].size(), 0.0)); // 线程私有
        for (size_t iblock = iblock_range.begin();
             iblock < iblock_range.end();
             iblock += blockSize)
        {
          for (size_t kblock = 0; kblock < src2.size(); kblock += blockSize)
          {
            for (size_t jblock = 0; jblock < dst.size(); jblock += blockSize)
            {
              for (size_t i = iblock;
                   i < min(iblock + blockSize, src1.size());
                   i++)
              {
                for (size_t k = kblock;
                     k < min(kblock + blockSize, src2.size());
                     k++)
                {
                  for (size_t j = jblock;
                       j < min(jblock + blockSize, dst.size());
                       j++)
                  {
                    local_dst[i][j] += src1[i][k] * src2[k][j];
                  }
                }
              }
            }
          }
        }
        // 归约阶段
        for (size_t i = start; i < end; i++)
        {
          for (size_t j = 0; j < dst[0].size(); j++)
          {
            dst[i][j] += local_dst[i][j]; // 这里的写入是有序的
          }
        }
      });
}

void MatrixBenchMark::parallel_computing_simple_multithread(
    std::vector<std::vector<int>> &matrix1,
    std::vector<std::vector<int>> &matrix2,
    std::vector<std::vector<int>> &result,
    size_t block_size)
{
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

void MatrixBenchMark::single_thread_matrix_mul_double(
    vector<vector<double>> &matrix1,
    vector<vector<double>> &matrix2,
    vector<vector<double>> &result,
    size_t block_size)
{
  matrix_mul(matrix1, matrix2, result, block_size, 0, matrix1.size());
}

void MatrixBenchMark::parallel_computing_openmp(vector<vector<int>> &src1,
                                                vector<vector<int>> &src2,
                                                vector<vector<int>> &dst,
                                                size_t blockSize)
{
  clear_matrix(dst);
#pragma omp parallel for collapse(2)
  for (size_t iblock = 0; iblock < src1.size(); iblock += blockSize)
  {
    for (size_t kblock = 0; kblock < src2.size(); kblock += blockSize)
    {
      for (size_t jblock = 0; jblock < dst[0].size(); jblock += blockSize)
      {
        for (size_t i = iblock; i < min(iblock + blockSize, src1.size()); i++)
        {
          for (size_t j = jblock; j < min(jblock + blockSize, dst[0].size());
               j++)
          {
            int sum = 0;
            for (size_t k = kblock; k < min(kblock + blockSize, src2.size());
                 k++)
            {
              sum += src1[i][k] * src2[k][j];
            }
#pragma omp atomic
            dst[i][j] += sum;
          }
        }
      }
    }
  }
}

#endif // PARALLEL_MATRIX_MUL_H
