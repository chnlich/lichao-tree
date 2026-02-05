CXX = g++
CXXFLAGS = -O3 -std=c++17 -Wall -Wextra

SRC = benchmark.cpp
BIN = benchmark_runner

# Include paths for headers in parent directories
INCLUDES = -I../implementation -I../generator

.PHONY: all clean run

all: $(BIN)

$(BIN): $(SRC) ../implementation/lichao.hpp ../implementation/cht.hpp ../generator/generator.hpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $(BIN) $(SRC)

clean:
	rm -f $(BIN) benchmark_results.csv

run: $(BIN)
	./$(BIN)
