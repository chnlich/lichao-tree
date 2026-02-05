CXX = g++
CXXFLAGS = -O3 -std=c++17 -Wall -Wextra

BENCHMARK_DIR = benchmark
BENCHMARK_SRC = $(BENCHMARK_DIR)/benchmark.cpp
BENCHMARK_BIN = $(BENCHMARK_DIR)/benchmark_runner

.PHONY: all clean run

all: $(BENCHMARK_BIN)

$(BENCHMARK_BIN): $(BENCHMARK_SRC) implementation/lichao.hpp implementation/cht.hpp generator/generator.hpp
	$(CXX) $(CXXFLAGS) -o $(BENCHMARK_BIN) $(BENCHMARK_SRC)

clean:
	rm -f $(BENCHMARK_BIN) $(BENCHMARK_DIR)/benchmark_results.csv

run: $(BENCHMARK_BIN)
	./$(BENCHMARK_BIN)
