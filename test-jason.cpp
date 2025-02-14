#include <iostream>
#include <chrono>

using namespace std;

void parallel_computing_openMP(vector<vector<int>> &src1,
                                vector<vector<int>> &src2,
                                vector<vector<int>> &dst) 
{
#pragma omp parallel for
  for (size_t i = 0; i < src1.size(); i++) {
    for (size_t k = 0; k < src2.size(); k++) {
      for (size_t j = 0; j < dst.size(); j++) {
        dst[i][k] += src1[i][j] * src2[j][k];
      }
    }
  }
}

void printMat(vector<vector<int>> &src) {
  for (size_t i = 0; i < src.size(); ++i) {
    for (size_t j = 0; j < src.size(); ++j) {
      cout << src[i][j] << " ";
    }
    cout << endl;
  }
}

int main() {
  // Default values
  size_t matrix_size = 512;

  // Initialize matrices
  vector<vector<int>> src1(matrix_size, vector<int>(matrix_size));
  vector<vector<int>> src2(matrix_size, vector<int>(matrix_size));
  vector<vector<int>> dst(matrix_size, vector<int>(matrix_size, 0));

  for (size_t row = 0; row < matrix_size; row++)
  {
    for (size_t col = 0; col < matrix_size; col++)
    {
      src1[row][col] = static_cast<int>(row) + static_cast<int>(col);
      src2[row][col] = static_cast<int>(row) + static_cast<int>(col);
    }
  }

  // printMat(src1);
  // printMat(src2);

  auto start_time = std::chrono::high_resolution_clock::now();
  
  parallel_computing_openMP(src1, src2, dst);

  auto end_time = chrono::high_resolution_clock::now();
  
  // printMat(dst);

  auto duration = chrono::duration_cast<std::chrono::microseconds>(
      end_time - start_time);
  double seconds = static_cast<double>(duration.count()) / 1000000.0;

  std::cout << "Time: " << seconds << '\n';

  return 0;
}
