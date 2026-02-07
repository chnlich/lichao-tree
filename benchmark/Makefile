CXX = g++
CXXFLAGS = -O3 -std=c++17 -Wall -Wextra

SRC = benchmark.cpp
BIN = benchmark_runner

EXP_SRC = benchmark_experiments.cpp
EXP_BIN = benchmark_experiments

# Include paths for headers in parent directories
INCLUDES = -I../implementation -I../generator

.PHONY: all clean run experiments

all: $(BIN) $(EXP_BIN)

$(BIN): $(SRC) ../implementation/lichao.hpp ../implementation/cht.hpp ../generator/generator.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN) $(SRC)

$(EXP_BIN): $(EXP_SRC) ../implementation/lichao.hpp ../implementation/cht.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(EXP_BIN) $(EXP_SRC)

clean:
	rm -f $(BIN) $(EXP_BIN) benchmark_results.csv experiment1_nc_results.csv experiment2_fp_results.csv

run: $(BIN)
	./$(BIN)

experiments: $(EXP_BIN)
	./$(EXP_BIN)
