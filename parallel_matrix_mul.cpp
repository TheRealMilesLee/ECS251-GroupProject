#include <chrono>
#include <future>
#include <iostream>
#include <thread>
#include <vector>
using namespace std;

/**
 * @brief This function performs matrix multiplication in parallel, utilizing
 * the blocking method with the block ik. For each thread, it takes one block
 * and performs the multiplication of the block with the entire matrix.
 *
 * The start and end are the range of the rows of the matrix.
 *
 * @param src1 The first matrix
 * @param src2 The second matrix
 * @param dst The resulting matrix
 * @param blockSize The size of the block
 * @param start The starting row index
 * @param end The ending row index
 */
void matrix_mul(vector<vector<int>> &src1, vector<vector<int>> &src2,
                vector<vector<int>> &dst, size_t blockSize, size_t start,
                size_t end);

/**
 * @brief This function is to parallelize the matrix multiplication using the
 * blocking method. It divides the matrix into blocks and then performs the
 * multiplication in parallel. e.g. If the matrix is 4096x4096, then it will
 * divide the matrix into 128x128 blocks and perform the matrix multiplication
 * on each block in parallel.
 *
 * For example, the thread1 looks like this:
 * src1:[0,127][0,127]
 * src2:[0,127][0,127]
 * dst:[0,127][0,127]
 * perform matrix multiplication and store in the dst matrix
 *
 * The thread2 looks like this:
 * src1:[128, 255][128, 255]
 * src2:[128, 255][128, 255]
 * dst:[128, 255][128, 255]
 * perform matrix multiplication and store in the dst matrix
 *
 * The block_per_thread is divided by the rows of the matrix.
 *
 * @param src1 The first matrix
 * @param src2 The second matrix
 * @param dst The resulting matrix
 * @param blockSize The size of the block
 */
void parallel_computing(vector<vector<int>> &src1, vector<vector<int>> &src2,
                        vector<vector<int>> &dst, size_t blockSize);

int main()
{
  // Set the block size, this number is based on my computer L1d cache size.
  // command lscpu | grep 'L1d cache' could get the L1d cache size, I found it
  // L1d cache size / 2 is the best block size.
  const int block_size = 128;

  // Create the matrices
  vector<vector<int>> src1(4096, vector<int>(4096, 0));
  vector<vector<int>> src2(4096, vector<int>(4096, 0));
  vector<vector<int>> dst(4096, vector<int>(4096, 0));

  // Slice the matrix into blocks
  const int num_blocks = 4096 / block_size;
  vector<vector<vector<int>>> src1_blocks(
      num_blocks, vector<vector<int>>(block_size, vector<int>(block_size)));
  vector<vector<vector<int>>> src2_blocks(
      num_blocks, vector<vector<int>>(block_size, vector<int>(block_size)));
  vector<vector<vector<int>>> dst_blocks(
      num_blocks, vector<vector<int>>(block_size, vector<int>(block_size)));

  // Fill the blocks with corresponding values from the original matrices
  for (size_t i = 0; i < num_blocks; ++i)
  {
    for (size_t j = 0; j < num_blocks; ++j)
    {
      for (size_t bi = 0; bi < block_size; ++bi)
      {
        for (size_t bj = 0; bj < block_size; ++bj)
        {
          src1_blocks[i][bi][bj] =
              src1[i * block_size + bi][j * block_size + bj];
          src2_blocks[i][bi][bj] =
              src2[i * block_size + bi][j * block_size + bj];
          dst_blocks[i][bi][bj] =
              dst[i * block_size + bi][j * block_size + bj];
        }
      }
    }
  }

  // Start timing
  auto start_time = std::chrono::high_resolution_clock::now();

  // Now pass these blocks to the parallel_computing function
  parallel_computing(src1, src2, dst, block_size);

  // End timing
  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
      end_time - start_time);

  double seconds = static_cast<double>(duration.count()) / 1000000.0;

  cout << "Matrix multiplication time: " << seconds << " seconds or "
       << duration.count() % 1000000 << " microseconds" << endl;
}

void matrix_mul(vector<vector<int>> &src1, vector<vector<int>> &src2,
                vector<vector<int>> &dst, size_t blockSize, size_t start,
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

void parallel_computing(vector<vector<int>> &src1, vector<vector<int>> &src2,
                        vector<vector<int>> &dst, size_t blockSize)
{
  // Get the number of processors
  const auto processor_count = thread::hardware_concurrency();
  // Using async to create a thread pool. Each thread will perform matrix
  // multiplication
  vector<future<void>> futures;
  // Calculate the number of blocks per thread
  size_t block_per_thread = src1.size() / processor_count;

  // Launch threads to perform matrix multiplication in parallel
  for (size_t i = 0; i < processor_count; i++)
  {
    // Calculate the block range for each thread
    size_t start = i * block_per_thread;
    // If it is the last thread, then the end is the size of the matrix
    size_t end =
        (i == processor_count - 1) ? src1.size() : (i + 1) * block_per_thread;

    futures.push_back(async(launch::async, matrix_mul, ref(src1), ref(src2),
                            ref(dst), blockSize, start, end));
  }

  // Wait for all threads to complete
  for (auto &f : futures)
  {
    f.get();
  }
}
