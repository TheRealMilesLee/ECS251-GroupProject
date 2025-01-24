#include "parallel_execute.h"
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char *argv[])
{
  parallel_execute pe;

  if (argc < 2)
  {
    cout << "Usage: " << argv[0]
         << " <job name> <job parameters> <num_threads>" << endl;
    return 1;
  }

  string job_name = argv[1];
  if (job_name == "matrix_multiply")
  {

    vector<vector<int>> matrix1;
    vector<vector<int>> matrix2;
    vector<vector<int>> result;
    int num_threads = atoi(argv[4]);

    // Start timing
    auto start_time = std::chrono::high_resolution_clock::now();

    pe.parallel_execute_matrix_multiplication(matrix1, matrix2, result);

    // End timing
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
        end_time - start_time);
    cout << "Matrix multiplication time: " << duration.count()
         << " microseconds" << endl;
  }
  else if (job_name == "sort")
  {
    vector<int> v;
    // Create a random vector of integers
    srand(time(0));        // Seed the random number generator
    const int size = 1000; // 1 million elements
    for (int i = 0; i < size; i++)
    {
      v.push_back(rand() % 10000); // Random numbers between 0 and 9999
    }

    int num_threads = atoi(argv[2]);
    unsigned int hw_threads = std::thread::hardware_concurrency();
    cout << "Hardware supports " << hw_threads << " concurrent threads"
         << endl;
    cout << "Sorting " << size << " elements using " << num_threads
         << " threads..." << endl;

    // Print first few elements before sorting
    cout << "First 10 elements before sorting:" << endl;
    for (int i = 0; i < min(10, size); i++)
    {
      cout << v[i] << " ";
    }
    cout << endl;

    // Start timing
    auto start_time = std::chrono::high_resolution_clock::now();

    pe.parallel_execute_sort(v, num_threads);

    // End timing
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
        end_time - start_time);
    cout << "Sorting time: " << duration.count() << " microseconds" << endl;
  }
  else if (job_name == "array_sum")
  {
    vector<int> v; // Deserialize argv[2] to vector v
    int num_threads = atoi(argv[3]);
    pe.parallel_execute_array_sum(v);
  }
  else if (job_name == "array_search")
  {
    int searchNum = atoi(argv[2]);
    cout << argv[0] << " " << argv[1] << " " << argv[2] << " " << argv[3]
         << endl;
    vector<int> v;
    for (int index = 3; index < argc; index++)
    {
      v.push_back(atoi(argv[index]));
    }

    // Start timing
    auto start_time = std::chrono::high_resolution_clock::now();

    pe.parallel_execute_array_search(v, searchNum);

    // End timing
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
        end_time - start_time);
    cout << "Array search time: " << duration.count() << " microseconds"
         << endl;
    cout << "v.size() = " << v.size() << endl;
    cout << "Index 0: " << v[0] << endl;
    pe.parallel_execute_array_search(v, searchNum);
  }
  else
  {
    cout << "Not implemented yet: " << job_name << endl;
    return 1;
  }
  return 0;
}
