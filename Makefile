CXX = g++
CXXFLAGS = -O3 -std=c++17 -Wall -Wextra

all: benchmark

benchmark: benchmark.cpp rb_lichao.hpp cht.hpp generator.hpp
	$(CXX) $(CXXFLAGS) -o benchmark benchmark.cpp

clean:
	rm -f benchmark benchmark_results.csv

run: benchmark
	./benchmark
