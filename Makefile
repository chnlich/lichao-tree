CXX = g++
CXXFLAGS = -O3 -std=c++17 -Wall -Wextra

all: benchmark

benchmark: main.cpp lichao.hpp cht.hpp generator.hpp
	$(CXX) $(CXXFLAGS) -o benchmark main.cpp

clean:
	rm -f benchmark
