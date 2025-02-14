CXX = clang++
CXXFLAGS = -Oz -g -pedantic-errors -Weverything -ltbb -Wno-poison-system-directories -Wthread-safety -Wno-c++98-compat -std=c++23 -pthread -lblas

SRC = matrix_mul.cpp parallel_matrix_mul_standard.cpp parallel_matrix_mul_async.cpp parallel_matrix_mul_fifo.cpp parallel_matrix_mul_lifo.cpp parallel_matrix_mul_tbb.cpp matrix_mul_blas.cpp
OBJ = $(SRC:.cpp=.o)

TARGETS = matrix_mul_single parallel_matrix_mul_standard matrix_mul_async matrix_mul_fifo matrix_mul_lifo matrix_mul_tbb matrix_mul_blas

all: $(TARGETS)

matrix_mul_single: matrix_mul.o
	$(CXX) $(CXXFLAGS) -o $@ $^

parallel_matrix_mul_standard: parallel_matrix_mul_standard.o
	$(CXX) $(CXXFLAGS) -o $@ $^

matrix_mul_async: parallel_matrix_mul_async.o
	$(CXX) $(CXXFLAGS) -o $@ $^

matrix_mul_fifo: parallel_matrix_mul_fifo.o
	$(CXX) $(CXXFLAGS) -o $@ $^

matrix_mul_lifo: parallel_matrix_mul_lifo.o
	$(CXX) $(CXXFLAGS) -o $@ $^

matrix_mul_tbb: parallel_matrix_mul_tbb.o
	$(CXX) $(CXXFLAGS) -o $@ $^

matrix_mul_blas: matrix_mul_blas.o
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGETS)
