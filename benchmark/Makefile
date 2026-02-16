CXX = g++
CXXFLAGS = -O3 -std=c++17 -Wall -Wextra

SRC = benchmark.cpp
BIN = benchmark_runner

EXP_SRC = benchmark_experiments.cpp
EXP_BIN = benchmark_experiments


# Include paths for headers in parent directories
INCLUDES = -I../implementation -I../generator

.PHONY: all clean run

all: $(BIN)

$(BIN): $(SRC) ../implementation/lict.hpp ../implementation/zkw_lict.hpp ../implementation/cht.hpp ../generator/generator.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN) $(SRC)

clean:
	rm -f $(BIN) benchmark_results.csv experiment1_nc_results.csv experiment2_fp_results.csv

run: $(BIN)
	./$(BIN)
