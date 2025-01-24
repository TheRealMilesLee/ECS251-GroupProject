#include <algorithm>
#include <atomic>
#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

class parallel_execute
{
private:
  std::vector<std::thread> threads;

public:
  void parallel_execute_matrix_multiplication(
      std::vector<std::vector<int>> &matrix1,
      std::vector<std::vector<int>> &matrix2,
      std::vector<std::vector<int>> &result);           // Michael
  void parallel_execute_sort(std::vector<int> &v);      // Zhuosheng
  void parallel_execute_array_sum(std::vector<int> &v); // Jason
  void parallel_execute_array_search(std::vector<int> &v, int key); // Hengyi
  void search_function(size_t start, size_t end, std::mutex &mtx,
                       std::atomic<int> &foundIndex,
                       const std::vector<int> &v); // 新增
};

void parallel_execute::parallel_execute_matrix_multiplication(
    std::vector<std::vector<int>> &matrix1,
    std::vector<std::vector<int>> &matrix2,
    std::vector<std::vector<int>> &result)
{
}
void parallel_execute::parallel_execute_sort(std::vector<int> &v) {}
void parallel_execute::parallel_execute_array_sum(std::vector<int> &v) {}

void parallel_execute::parallel_execute_array_search(std::vector<int> &v,
                                                     int key)
{
  std::mutex mtx;
  std::atomic<int> foundIndex(-1);

  // Get the number of threads supported by the hardware
  unsigned int numThreads = std::thread::hardware_concurrency();
  if (numThreads == 0)
  {
    numThreads = 8; // Default to 8 threads
  }

  // Calculate the chunk size
  size_t chunkSize = v.size() / numThreads;

  // Clear the threads vector
  threads.clear();

  for (unsigned int i = 0; i < numThreads; ++i)
  {
    size_t start = i * chunkSize;
    size_t end = (i == numThreads - 1) ? v.size() : start + chunkSize;

    threads.emplace_back(&parallel_execute::search_function, this, start, end,
                         std::ref(mtx), std::ref(foundIndex), std::ref(v));
  }

  for (auto &thread : threads)
  {
    thread.join();
  }

  if (foundIndex == -1)
  {
    std::cout << "Key " << key << " not found in the array." << std::endl;
  }
  else
  {
    std::cout << "Found key " << key << " at index " << foundIndex
              << std::endl;
  }
}

void parallel_execute::search_function(size_t start, size_t end,
                                       std::mutex &mtx,
                                       std::atomic<int> &foundIndex,
                                       const std::vector<int> &v)
{
  for (size_t index = start; index < end && foundIndex == -1; ++index)
  {
    if (v[index] != -1)
    {
      std::lock_guard<std::mutex> lock(mtx);
      foundIndex = index;
      break;
    }
  }
}
