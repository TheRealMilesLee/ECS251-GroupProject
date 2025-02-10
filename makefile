CXX = clang++
CXXFLAGS = -Oz -pedantic-errors -Weverything -Wno-poison-system-directories -Wthread-safety -Wno-c++98-compat -std=c++23 -pthread

SRC = matrix_mul.cpp parallel_matrix_mul.cpp
OBJ = $(SRC:.cpp=.o)

TARGETS = matrix_mul_single matrix_multiply_parallel

all: $(TARGETS)

matrix_mul_single: matrix_mul.o
	$(CXX) $(CXXFLAGS) -o $@ $^


matrix_multiply_parallel: parallel_matrix_mul.o
	$(CXX) $(CXXFLAGS) -o $@ $^


%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGETS) Results/gmon_matrix_mul.out Results/gmon_parallel_matrix_mul.out
