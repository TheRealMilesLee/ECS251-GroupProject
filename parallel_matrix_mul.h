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
  void matrix_mul(vector<vector<int>> &src1,
                  vector<vector<int>> &src2,
                  vector<vector<int>> &dst,
                  size_t blockSize,
                  size_t start,
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
                                vector<vector<int>> &dst,
                                size_t blockSize);

  /**
   * @brief Performs parallel matrix multiplication using FIFO scheduling.
   *
   * This function multiplies two matrices (src1 and src2) and stores the result in the destination matrix (dst).
   * The multiplication is done in parallel using a block-based approach with FIFO scheduling.
   *
   * @param src1 The first source matrix.
   * @param src2 The second source matrix.
   * @param dst The destination matrix to store the result of the multiplication.
   * @param blockSize The size of the blocks to be used for parallel computation.
   */
  void parallel_computing_fifo(vector<vector<int>> &src1,
                               vector<vector<int>> &src2,
                               vector<vector<int>> &dst,
                               size_t blockSize);

  /**
   * @brief Performs parallel matrix multiplication using a LIFO (Last In, First Out) approach.
   *
   * This function multiplies two matrices (src1 and src2) and stores the result in the destination matrix (dst).
   * The multiplication is performed in parallel using blocks of a specified size.
   *
   * @param src1 The first source matrix to be multiplied.
   * @param src2 The second source matrix to be multiplied.
   * @param dst The destination matrix where the result will be stored.
   * @param blockSize The size of the blocks used for parallel computation.
   */
  void parallel_computing_lifo(vector<vector<int>> &src1,
                               vector<vector<int>> &src2,
                               vector<vector<int>> &dst,
                               size_t blockSize);

  /**
   * @brief Performs parallel matrix multiplication using the Parallel Patterns Library (PPL).
   *
   * This function multiplies two matrices (src1 and src2) and stores the result in the destination matrix (dst).
   * The multiplication is performed in parallel using blocks of the specified size.
   *
   * @param src1 The first source matrix.
   * @param src2 The second source matrix.
   * @param dst The destination matrix to store the result of the multiplication.
   * @param blockSize The size of the blocks to be used for parallel computation.
   */
  void parallel_computing_ppl(vector<vector<int>> &src1,
                              vector<vector<int>> &src2,
                              vector<vector<int>> &dst,
                              size_t blockSize);

  /**
   * @brief Performs parallel matrix multiplication using Intel Threading Building Blocks (TBB).
   *
   * This function multiplies two matrices (src1 and src2) and stores the result in the destination matrix (dst).
   * The multiplication is performed in parallel using TBB to improve performance.
   *
   * @param src1 The first source matrix.
   * @param src2 The second source matrix.
   * @param dst The destination matrix where the result will be stored.
   * @param blockSize The size of the blocks used for parallel computation.
   */
  void parallel_computing_tbb(vector<vector<int>> &src1,
                              vector<vector<int>> &src2,
                              vector<vector<int>> &dst,
                              size_t blockSize);
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
  // Get the number of available threads
  size_t num_threads = thread::hardware_concurrency();

  // Create a task group for parallel execution
  vector<thread> threads;
  mutex task_mutex;
  atomic<size_t> next_block{ 0 };

  // Worker function
  auto worker = [&]()
  {
    while (true)
    {
      size_t block;
      {
        lock_guard<mutex> lock(task_mutex);
        block = next_block++;
        if (block >= src1.size())
        {
          break;
        }
      }
      matrix_mul(src1,
                 src2,
                 dst,
                 blockSize,
                 block,
                 min(block + blockSize, src1.size()));
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

void MatrixBenchMark::parallel_computing_tbb(vector<vector<int>> &src1,
                                             vector<vector<int>> &src2,
                                             vector<vector<int>> &dst,
                                             size_t blockSize)
{
  // Get the number of available threads
  size_t num_threads = thread::hardware_concurrency();

  // Create a thread pool
  vector<thread> threads;
  mutex task_mutex;
  atomic<size_t> next_block{ 0 };

  // Worker function
  auto worker = [&]()
  {
    while (true)
    {
      size_t block;
      {
        lock_guard<mutex> lock(task_mutex);
        block = next_block++;
        if (block >= src1.size())
        {
          break;
        }
      }
      matrix_mul(src1,
                 src2,
                 dst,
                 blockSize,
                 block,
                 min(block + blockSize, src1.size()));
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

#endif // PARALLEL_MATRIX_MUL_H
