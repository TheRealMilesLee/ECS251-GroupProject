#include <algorithm>
#include <iostream>
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
      std::vector<std::vector<int>> &result);
  void parallel_execute_sort(std::vector<int> &v);
  void parallel_execute_array_sum(std::vector<int> &v);
  void parallel_execute_array_search(std::vector<int> &v, int key);
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
}
