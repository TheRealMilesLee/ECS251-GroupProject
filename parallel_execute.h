#include <algorithm>
#include <climits>
#include <iostream>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>
#include <atomic>

class parallel_execute
{
private:
  std::vector<std::thread> threads;

public:
  void parallel_execute_matrix_multiplication(
      std::vector<std::vector<int>> &matrix1,
      std::vector<std::vector<int>> &matrix2,
      std::vector<std::vector<int>> &result); // Michael
  void parallel_execute_sort(std::vector<int> &v,
                             int num_threads);          // Zhuosheng
  void parallel_execute_array_sum(std::vector<int> &v); // Jason
  void parallel_execute_array_search(std::vector<int> &v, int key); // Hengyi
  void search_function(size_t start, size_t end, std::mutex &mtx,
                       std::atomic<int> &foundIndex,
                       const std::vector<int> &v, int key);
  void merge_function(std::vector<int> &v,
                      std::vector<std::vector<int>> &sortedChunks);
};

void parallel_execute::parallel_execute_sort(std::vector<int> &v,
                                             int num_threads)
{
  if (v.empty())
  {
    return;
  }
  if (num_threads <= 0)
  {
    num_threads = 1;
  }

  // Calculate chunk size
  size_t chunkSize = (v.size() + num_threads - 1) / num_threads;

  // Clear previous threads
  threads.clear();

  // Create a copy of the input vector to avoid data races
  std::vector<int> input_copy = v;
  size_t actual_threads =
      std::min(static_cast<size_t>(num_threads), v.size());
  std::vector<std::vector<int>> sortedChunks(actual_threads);

  // Sort chunks in parallel
  for (int i = 0; i < actual_threads; ++i)
  {
    size_t start = i * chunkSize;
    size_t end = std::min(start + chunkSize, input_copy.size());

    threads.emplace_back(
        [&input_copy, &sortedChunks, i, start, end]()
        {
          // Create and sort chunk
          sortedChunks[i] = std::vector<int>(input_copy.begin() + start,
                                             input_copy.begin() + end);
          std::sort(sortedChunks[i].begin(), sortedChunks[i].end());
        });
  }

  // Wait for all threads to complete
  for (auto &thread : threads)
  {
    thread.join();
  }

  // Merge sorted chunks
  v.clear();
  v.reserve(input_copy.size());
  merge_function(v, sortedChunks);
}

void parallel_execute::parallel_execute_matrix_multiplication(
    std::vector<std::vector<int>> &matrix1,
    std::vector<std::vector<int>> &matrix2,
    std::vector<std::vector<int>> &result)
{
}

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

    // 修改: 传递key参数
    threads.emplace_back(&parallel_execute::search_function, this, start, end,
                         std::ref(mtx), std::ref(foundIndex), std::ref(v),
                         key);
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
                                       const std::vector<int> &v, int key)
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
}

void parallel_execute::merge_function(
    std::vector<int> &v, std::vector<std::vector<int>> &sortedChunks)
{
  // Priority queue for merging
  using HeapElement =
      std::pair<int, std::pair<size_t, size_t>>; // (value, (chunk_index,
                                                 // position))
  std::vector<HeapElement> heap;

  // Initialize heap with first element from each chunk
  for (size_t i = 0; i < sortedChunks.size(); ++i)
  {
    if (!sortedChunks[i].empty())
    {
      heap.emplace_back(sortedChunks[i][0], std::make_pair(i, 0));
    }
  }

  // Create min-heap
  auto comp = [](const HeapElement &a, const HeapElement &b)
  { return a.first > b.first; };
  std::make_heap(heap.begin(), heap.end(), comp);

  // Merge using min-heap
  while (!heap.empty())
  {
    std::pop_heap(heap.begin(), heap.end(), comp);
    auto current = heap.back();
    heap.pop_back();

    v.push_back(current.first);

    size_t chunk_idx = current.second.first;
    size_t pos = current.second.second;

    if (pos + 1 < sortedChunks[chunk_idx].size())
    {
      heap.emplace_back(sortedChunks[chunk_idx][pos + 1],
                        std::make_pair(chunk_idx, pos + 1));
      std::push_heap(heap.begin(), heap.end(), comp);
    }
  }
}
