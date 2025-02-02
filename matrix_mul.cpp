#include <algorithm>
#include <chrono>
#include <iostream>
#include <vector>
using namespace std;

int matrix_multiplication(vector<vector<int>> &matrix1,
                          vector<vector<int>> &matrix2,
                          vector<vector<int>> &result);

int main()
{
  vector<vector<int>> v(1024, vector<int>(1024));
  for (size_t i = 0; i < 1024; i++)
  {
    for (size_t j = 0; j < 1024; j++)
    {
      v[i][j] = rand();
    }
  }

  vector<vector<int>> v2(1024, vector<int>(1024));
  for (size_t i = 0; i < 1024; i++)
  {
    for (size_t j = 0; j < 1024; j++)
    {
      v2[i][j] = rand();
    }
  }

  vector<vector<int>> v3(1024, vector<int>(1024, 0));

  // Start timing
  auto start_time = std::chrono::high_resolution_clock::now();

  matrix_multiplication(v, v2, v3);
  // End timing
  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
      end_time - start_time);
  // convert microseconds to seconds
  int seconds = duration.count() / 1000000;

  cout << "Matrix multiplication time: " << seconds << " seconds or "
       << duration.count() % 1000000 << " microseconds" << endl;
}

int matrix_multiplication(vector<vector<int>> &matrix1,
                          vector<vector<int>> &matrix2,
                          vector<vector<int>> &result)
{
  size_t blockSize = 64;
  size_t n = matrix1.size();

  for (size_t i = 0; i < n; i += blockSize)
  {
    for (size_t j = 0; j < n; j += blockSize)
    {
      for (size_t k = 0; k < n; k += blockSize)
      {
        for (size_t ii = i; ii < min(i + blockSize, n); ii++)
        {
          for (size_t jj = j; jj < min(j + blockSize, n); jj++)
          {
            for (size_t kk = k; kk < min(k + blockSize, n); kk++)
            {
              result[ii][jj] += matrix1[ii][kk] * matrix2[kk][jj];
            }
          }
        }
      }
    }
  }
  return 0;
}
