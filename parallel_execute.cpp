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
    pe.parallel_execute_matrix_multiplication(matrix1, matrix2, result);
  }
  else if (job_name == "sort")
  {
    vector<int> v; // Deserialize argv[2] to vector v
    int num_threads = atoi(argv[3]);
    pe.parallel_execute_sort(v);
  }
  else if (job_name == "array_sum")
  {
    vector<int> v; // Deserialize argv[2] to vector v
    int num_threads = atoi(argv[3]);
    pe.parallel_execute_array_sum(v);
  }
  else if (job_name == "array_search")
  {
    vector<int> v; // Deserialize argv[2] to vector v
    int key = atoi(argv[3]);
    int num_threads = atoi(argv[4]);
    pe.parallel_execute_array_search(v, key);
  }
  else
  {
    cout << "Not implemented yet: " << job_name << endl;
    return 1;
  }
  return 0;
}
