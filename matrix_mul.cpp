#include <algorithm>
#include <chrono>
#include <iostream>
#include <vector>
using namespace std;

/**
 * This function is for the matrix multiplication in the default standards.
 * @param matrix1 The first matrix to be multiplied.
 * @param matrix2 The second matrix to be multiplied.
 * @param result The matrix that will store the result of the multiplication.
 * @return 0 if the multiplication is successful.
 */
int matrix_multiplication(vector<vector<int>> &matrix1,
                          vector<vector<int>> &matrix2,
                          vector<vector<int>> &result, size_t blockSize);

int main()
{
  vector<vector<int>> v(4096, vector<int>(4096));
  for (size_t i = 0; i < 4096; i++)
  {
    for (size_t j = 0; j < 4096; j++)
    {
      v[i][j] = rand();
    }
  }

  vector<vector<int>> v2(4096, vector<int>(4096));
  for (size_t i = 0; i < 4096; i++)
  {
    for (size_t j = 0; j < 4096; j++)
    {
      v2[i][j] = rand();
    }
  }

  vector<vector<int>> v3(4096, vector<int>(4096, 0));

  // Start timing
  auto start_time = std::chrono::high_resolution_clock::now();

  matrix_multiplication(v, v2, v3, 128);
  // End timing
  auto end_time = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(
      end_time - start_time);

  double seconds = static_cast<double>(duration.count()) / 1000000.0;

  cout << "Matrix multiplication time: " << seconds << " seconds or "
       << duration.count() % 1000000 << " microseconds" << endl;
}

int matrix_multiplication(vector<vector<int>> &matrix1,
                          vector<vector<int>> &matrix2,
                          vector<vector<int>> &result, size_t blockSize)
{
  size_t n = matrix1.size();

  for (size_t ii = 0; ii < n; ii += blockSize)
  {
    for (size_t kk = 0; kk < n; kk += blockSize)
    {
      for (size_t j = 0; j < n; j++)
      {
        for (size_t i = ii; i < min(ii + blockSize, n); i++)
        {
          for (size_t k = kk; k < min(kk + blockSize, n); k++)
          {
            result[i][j] += matrix1[i][k] * matrix2[k][j];
          }
        }
      }
    }
  }
  return 0;
}
