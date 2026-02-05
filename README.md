# Li-Chao Tree (LICT) Simplified Repository

A streamlined collection of the two most competitive implementations for Dynamic Convex Hull Optimization in C++.

## Overview

This repository provides optimized implementations for maintaining the lower envelope of a set of linear functions (lines) $y = kx + m$, supporting efficient line insertion and point query operations.

## Implementations

| Implementation | Header File | Description | Time Complexity |
|---------------|-------------|-------------|-----------------|
| **LICT (Standard)** | `lichao.hpp` | Classic pointer-based segment tree implementation | $O(\log C)$ insertion/query |
| **Dynamic CHT** | `cht.hpp` | Classic Convex Hull Trick with balanced BST | $O(\log n)$ insertion/query |

## Structure

```
lichao-tree/
├── README.md                    - This file
├── Makefile                     - Root makefile (delegates to benchmark/)
├── .gitignore                   - Git ignore rules
├── implementation/
│   ├── lichao.hpp               - Standard Li-Chao Tree
│   └── cht.hpp                  - Dynamic CHT implementation
├── generator/
│   └── generator.hpp            - Test data generator
├── benchmark/
│   ├── Makefile                 - Benchmark-specific makefile
│   ├── benchmark.cpp            - Benchmark source
│   └── benchmark_results.csv    - Generated results
└── doc/
    ├── report.tex               - LaTeX report
    └── report.pdf               - Compiled PDF
```

## Usage

### Building

```bash
make
```

### Running Benchmarks

```bash
make run
```

Or manually:

```bash
g++ -O3 -std=c++17 -o benchmark benchmark.cpp
./benchmark
```

### Using LICT (Standard)

```cpp
#include "implementation/lichao.hpp"

LC::LiChaoTree lict(-1e9, 1e9);  // Create with coordinate range
lict.add_line(2, 3);  // Add line y = 2x + 3
long long val = lict.query(5);  // Query at x = 5, returns 13
```

### Using Dynamic CHT

```cpp
#include "implementation/cht.hpp"

CHT::DynamicCHT cht;
cht.add_line(2, 3);  // Add line y = 2x + 3
long long val = cht.query(5);  // Query at x = 5, returns 13
```

## Benchmarks

The benchmark compares both implementations across:
- **N values:** $10^5$, $10^6$, $10^7$ operations
- **Distributions:**
  - **Random:** Random slopes and intercepts
  - **All on Hull:** Lines constructed so all contribute to lower envelope

### Expected Performance

Based on empirical testing:

| N | Distribution | LICT (Standard) | Dynamic CHT |
|---|--------------|-----------------|-------------|
| $10^5$ | Random | ~7 ms | ~5 ms |
| $10^6$ | Random | ~76 ms | ~53 ms |
| $10^7$ | Random | ~766 ms | ~520 ms |

Both implementations provide excellent performance with logarithmic complexity per operation.

## When to Use Which?

- **Use LICT (Standard)** when:
  - You need the simplest, most reliable implementation
  - Working with integer coordinates and want to avoid floating-point precision issues
  - Need to handle arbitrary coordinate ranges easily
  - Want extensibility (segments, persistence) in the future

- **Use Dynamic CHT** when:
  - You need maximum performance
  - Comfortable with intersection point calculations
  - Working with lines where only a subset remain on the hull

## Technical Details

### Standard LICT
The Standard Li-Chao Tree is a binary segment tree where each node stores the line that is optimal at the midpoint of its interval. Insertion and query both take $O(\log C)$ time where $C$ is the coordinate range.

**Key advantages:**
- Simple integer comparisons (no floating-point)
- Easy to extend to line segments
- Natural support for persistence
- Works with any coordinate range

### Dynamic CHT
The Dynamic CHT uses a balanced BST (std::multiset) to maintain the convex hull explicitly. Insertion and query take $O(\log n)$ time where $n$ is the number of lines.

**Key advantages:**
- Better performance when few lines remain on hull
- Lower memory usage for sparse hulls
- Directly stores only relevant lines

## History

The Li-Chao Tree was introduced by **Li Chao** at ZJOI 2012. The Dynamic CHT is a classic competitive programming technique using balanced BSTs.

---
*Simplified and maintained for competitive programming and algorithm optimization.*
