#include <vector>
#include <thread>
#include <algorithm>
#include <climits>
#include <utility>

class parallel_execute {
private:
    std::vector<std::thread> threads;

public:
    void parallel_execute_matrix_multiplication(
        std::vector<std::vector<int>> &matrix1,
        std::vector<std::vector<int>> &matrix2,
        std::vector<std::vector<int>> &result);
    void parallel_execute_sort(std::vector<int> &v, int num_threads);
    void parallel_execute_array_sum(std::vector<int> &v);
    void parallel_execute_array_search(std::vector<int> &v, int key);
};

void parallel_execute::parallel_execute_sort(std::vector<int> &v, int num_threads) {
    if (v.empty()) return;

    // Calculate chunk size
    size_t chunkSize = (v.size() + num_threads - 1) / num_threads;

    // Clear previous threads
    threads.clear();

    // Create a copy of the input vector to avoid data races
    std::vector<int> input_copy = v;

    // Vector to store sorted chunks
    std::vector<std::vector<int>> sortedChunks(num_threads);

    // Sort chunks in parallel
    for (int i = 0; i < num_threads; ++i) {
        size_t start = i * chunkSize;
        size_t end = std::min(start + chunkSize, input_copy.size());
        
        threads.emplace_back([&input_copy, &sortedChunks, i, start, end]() {
            // Create and sort chunk
            sortedChunks[i] = std::vector<int>(input_copy.begin() + start, input_copy.begin() + end);
            std::sort(sortedChunks[i].begin(), sortedChunks[i].end());
        });
    }

    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }

    // Merge sorted chunks
    v.clear();
    v.reserve(input_copy.size());

    // Priority queue for merging
    using HeapElement = std::pair<int, std::pair<size_t, size_t>>; // (value, (chunk_index, position))
    std::vector<HeapElement> heap;

    // Initialize heap with first element from each chunk
    for (size_t i = 0; i < sortedChunks.size(); ++i) {
        if (!sortedChunks[i].empty()) {
            heap.emplace_back(sortedChunks[i][0], std::make_pair(i, 0));
        }
    }

    // Create min-heap
    auto comp = [](const HeapElement& a, const HeapElement& b) { return a.first > b.first; };
    std::make_heap(heap.begin(), heap.end(), comp);

    // Merge using min-heap
    while (!heap.empty()) {
        std::pop_heap(heap.begin(), heap.end(), comp);
        auto current = heap.back();
        heap.pop_back();

        v.push_back(current.first);

        size_t chunk_idx = current.second.first;
        size_t pos = current.second.second;

        if (pos + 1 < sortedChunks[chunk_idx].size()) {
            heap.emplace_back(sortedChunks[chunk_idx][pos + 1], std::make_pair(chunk_idx, pos + 1));
            std::push_heap(heap.begin(), heap.end(), comp);
        }
    }
}

void parallel_execute::parallel_execute_matrix_multiplication(
    std::vector<std::vector<int>> &matrix1,
    std::vector<std::vector<int>> &matrix2,
    std::vector<std::vector<int>> &result) {}

void parallel_execute::parallel_execute_array_sum(std::vector<int> &v) {}

void parallel_execute::parallel_execute_array_search(std::vector<int> &v, int key) {
    if (v.empty()) return;

    // Get the number of threads supported by the hardware
    unsigned int numThreads = std::thread::hardware_concurrency();
    if (numThreads == 0) numThreads = 8; // Default value

    // Calculate chunk size
    size_t chunkSize = (v.size() + numThreads - 1) / numThreads;

    // Clear previous threads
    threads.clear();

    // Variable to store the found index, -1 if not found
    int foundIndex = -1;
    bool found = false;

    // Search chunks in parallel
    for (unsigned int i = 0; i < numThreads; ++i) {
        size_t start = i * chunkSize;
        size_t end = std::min(start + chunkSize, v.size());

        threads.emplace_back([&v, key, start, end, &foundIndex, &found]() {
            for (size_t index = start; index < end && !found; ++index) {
                if (v[index] == key) {
                    foundIndex = index;
                    found = true;
                    break;
                }
            }
        });
    }

    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
}