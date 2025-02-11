#ifndef PARALLEL_MATRIX_MUL_H
#define PARALLEL_MATRIX_MUL_H
#include <future>
#include <thread>
#include <vector>
#include <queue>
#include <stack>
#include <mutex>
#include <tbb/tbb.h>
#include <tbb/parallel_for.h>

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
                  std::vector<std::vector<int>> &result,
                  size_t block_size,
                  size_t start,
                  size_t end);

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
 * @brief Multiplies two matrices using a single thread.
 *
 * @param matrix1 The first input matrix.
 * @param matrix2 The second input matrix.
 * @param result The resulting matrix after multiplication.
 * @param block_size The size of the block to be used for multiplication.
 */
  void single_thread_computing(std::vector<std::vector<int>> &matrix1,
                               std::vector<std::vector<int>> &matrix2,
                               std::vector<std::vector<int>> &result,
                               size_t block_size);

  /**
 * @brief Multiplies two matrices in parallel using a FIFO (First In, First Out) scheduling policy.
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
 * @brief Multiplies two matrices in parallel using a LIFO (Last In, First Out) scheduling policy.
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
 * @brief Multiplies two matrices in parallel using the Parallel Patterns Library (PPL).
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

  /**
 * @brief Multiplies two matrices in parallel using Intel Threading Building Blocks (TBB).
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
};

void MatrixBenchMark::matrix_mul(vector<vector<int>> &src1,
                                 vector<vector<int>> &src2,
                                 vector<vector<int>> &dst,
                                 size_t blockSize,
                                 size_t start,
                                 size_t end)
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

  // Calculate the total number of blocks
  size_t total_blocks = (src1.size() + blockSize - 1) / blockSize;

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

void MatrixBenchMark::parallel_computing_fifo(vector<vector<int>> &src1,
                                              vector<vector<int>> &src2,
                                              vector<vector<int>> &dst,
                                              size_t blockSize)
{
  // Get the number of available threads
  size_t num_threads = thread::hardware_concurrency();

  // Create a queue for tasks
  queue<pair<size_t, size_t>> task_queue;

  // Fill the queue with tasks (blocks)
  for (size_t i = 0; i < src1.size(); i += blockSize)
  {
    task_queue.push({ i, min(i + blockSize, src1.size()) });
  }

  // Create worker threads
  vector<thread> threads;
  mutex queue_mutex;

  // Worker function
  auto worker = [&]()
  {
    while (true)
    {
      pair<size_t, size_t> task;
      {
        lock_guard<mutex> lock(queue_mutex);
        if (task_queue.empty())
        {
          break;
        }
        task = task_queue.front();
        task_queue.pop();
      }
      matrix_mul(src1, src2, dst, blockSize, task.first, task.second);
    }
  };

  // Launch worker threads
  for (size_t i = 0; i < num_threads; ++i)
  {
    threads.emplace_back(worker);
  }

  // Wait for all threads to complete
  for (auto &thread : threads)
  {
    thread.join();
  }
}

void MatrixBenchMark::parallel_computing_lifo(vector<vector<int>> &src1,
                                              vector<vector<int>> &src2,
                                              vector<vector<int>> &dst,
                                              size_t blockSize)
{
  // Get the number of available threads
  size_t num_threads = thread::hardware_concurrency();

  // Create a stack for tasks
  stack<pair<size_t, size_t>> task_stack;

  // Fill the stack with tasks (blocks)
  for (size_t i = 0; i < src1.size(); i += blockSize)
  {
    task_stack.push({ i, min(i + blockSize, src1.size()) });
  }

  // Create worker threads
  vector<thread> threads;
  mutex stack_mutex;

  // Worker function
  auto worker = [&]()
  {
    while (true)
    {
      pair<size_t, size_t> task;
      {
        lock_guard<mutex> lock(stack_mutex);
        if (task_stack.empty())
        {
          break;
        }
        task = task_stack.top();
        task_stack.pop();
      }
      matrix_mul(src1, src2, dst, blockSize, task.first, task.second);
    }
  };

  // Launch worker threads
  for (size_t i = 0; i < num_threads; ++i)
  {
    threads.emplace_back(worker);
  }

  // Wait for all threads to complete
  for (auto &thread : threads)
  {
    thread.join();
  }
}

void MatrixBenchMark::parallel_computing_ppl(vector<vector<int>> &src1,
                                             vector<vector<int>> &src2,
                                             vector<vector<int>> &dst,
                                             size_t blockSize)
{
#ifdef _WIN32
  // Get the number of available threads
  size_t num_threads = thread::hardware_concurrency();

  // Create a task group for parallel execution
  concurrency::task_group tasks;

  // Create tasks for each block
  for (size_t i = 0; i < src1.size(); i += blockSize)
  {
    tasks.run(
        [&, i]()
        {
          matrix_mul(
              src1, src2, dst, blockSize, i, min(i + blockSize, src1.size()));
        });
  }

  // Wait for all tasks to complete
  tasks.wait();
#endif
}

void MatrixBenchMark::parallel_computing_tbb(vector<vector<int>> &src1,
                                             vector<vector<int>> &src2,
                                             vector<vector<int>> &dst,
                                             size_t blockSize)
{
  // Use TBB's parallel_for with blocked_range
  tbb::parallel_for(
      tbb::blocked_range<size_t>(0, src1.size(), blockSize),
      [&](const tbb::blocked_range<size_t> &range)
      {
        matrix_mul(src1, src2, dst, blockSize, range.begin(), range.end());
      });
}

void MatrixBenchMark::single_thread_computing(vector<vector<int>> &src1,
                                              vector<vector<int>> &src2,
                                              vector<vector<int>> &dst,
                                              size_t blockSize)
{
  // Process blocks sequentially
  for (size_t i = 0; i < src1.size(); i += blockSize)
  {
    matrix_mul(
        src1, src2, dst, blockSize, i, min(i + blockSize, src1.size()));
  }
}

#endif // PARALLEL_MATRIX_MUL_H
