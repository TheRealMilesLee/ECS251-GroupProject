CXX = clang++
CXXFLAGS = -Oz -pg -pedantic-errors -Weverything -ltbb -Wno-poison-system-directories -Wthread-safety -Wno-c++98-compat -std=c++23 -pthread

SRC = matrix_mul.cpp parallel_matrix_mul.cpp parallel_matrix_mul_async.cpp parallel_matrix_mul_fifo.cpp parallel_matrix_mul_lifo.cpp parallel_matrix_mul_tbb.cpp parallel_matrix_mul_ppl.cpp
OBJ = $(SRC:.cpp=.o)

TARGETS = matrix_mul_single matrix_multiply_parallel parallel_matrix_mul_async parallel_matrix_mul_fifo parallel_matrix_mul_lifo parallel_matrix_mul_tbb parallel_matrix_mul_ppl

all: $(TARGETS)

matrix_mul_single: matrix_mul.o
	$(CXX) $(CXXFLAGS) -o $@ $^

matrix_multiply_parallel: parallel_matrix_mul.o
	$(CXX) $(CXXFLAGS) -o $@ $^

parallel_matrix_mul_async: parallel_matrix_mul_async.o
	$(CXX) $(CXXFLAGS) -o $@ $^

parallel_matrix_mul_fifo: parallel_matrix_mul_fifo.o
	$(CXX) $(CXXFLAGS) -o $@ $^

parallel_matrix_mul_lifo: parallel_matrix_mul_lifo.o
	$(CXX) $(CXXFLAGS) -o $@ $^

parallel_matrix_mul_tbb: parallel_matrix_mul_tbb.o
	$(CXX) $(CXXFLAGS) -o $@ $^

parallel_matrix_mul_ppl: parallel_matrix_mul_ppl.o
	$(CXX) $(CXXFLAGS) -o $@ $^




%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGETS) Results/gmon_matrix_mul.out Results/gmon_parallel_matrix_mul.out
