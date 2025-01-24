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

  // 获取硬件支持的线程数量
  unsigned int numThreads = std::thread::hardware_concurrency();
  if (numThreads == 0)
    numThreads = 4; // 默认值

  // 计算每个线程处理的区间大小
  size_t chunkSize = v.size() / numThreads;

  // 清空之前的线程
  threads.clear();

  for (unsigned int i = 0; i < numThreads; ++i)
  {
    size_t start = i * chunkSize;
    size_t end = (i == numThreads - 1) ? v.size() : start + chunkSize;

    threads.emplace_back(
        [&, start, end]()
        {
          for (size_t index = start; index < end && foundIndex == -1; ++index)
          {
            if (v[index] == key)
            {
              std::lock_guard<std::mutex> lock(mtx);
              foundIndex = index;
              break;
            }
          }
        });
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

int searchIndex(int searchNum, int currentIndex, std::vector<int> &v)
{
  if (v[currentIndex] == searchNum)
  {
    return currentIndex;
  }
}
