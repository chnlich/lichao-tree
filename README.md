# Li-Chao Tree (LICT) Simplified Repository

A streamlined collection of the two most competitive implementations for Dynamic Convex Hull Optimization in C++.

## Overview

This repository provides optimized implementations for maintaining the lower envelope of a set of linear functions (lines) $y = kx + m$, supporting efficient line insertion and point query operations.

## Implementations

| Implementation | Header File | Description | Time Complexity |
|---------------|-------------|-------------|-----------------|
| **LICT (RB/Set)** | `rb_lichao.hpp` | Fastest Li-Chao variant using balanced BST (std::set) | $O(\log n)$ insertion/query |
| **Dynamic CHT** | `cht.hpp` | Classic Convex Hull Trick with balanced BST | $O(\log n)$ insertion/query |

## Files

- `rb_lichao.hpp` - Red-Black Tree based LICT (fastest variant)
- `cht.hpp` - Dynamic CHT implementation (balanced BST-based)
- `generator.hpp` - Test data generator with multiple distributions
- `benchmark.cpp` - Benchmark comparing the two implementations
- `report.tex` - LaTeX report with analysis

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

### Using LICT (RB/Set)

```cpp
#include "rb_lichao.hpp"

RB_LC::RBLiChaoTree lict;
lict.add_line(2, 3);  // Add line y = 2x + 3
long long val = lict.query(5);  // Query at x = 5, returns 13
```

### Using Dynamic CHT

```cpp
#include "cht.hpp"

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

| N | Distribution | LICT (RB/Set) | Dynamic CHT |
|---|--------------|---------------|-------------|
| $10^5$ | Random | ~5 ms | ~5 ms |
| $10^6$ | Random | ~55 ms | ~53 ms |
| $10^7$ | Random | ~540 ms | ~520 ms |

Both implementations provide excellent performance with $O(\log n)$ complexity per operation.

## When to Use Which?

- **Use LICT (RB/Set)** when:
  - You need the simplest, most reliable implementation
  - Working with integer coordinates only
  - Want to avoid floating-point precision issues

- **Use Dynamic CHT** when:
  - You need absolute maximum performance
  - Comfortable with intersection point calculations
  - Working with lines that may have similar slopes

## History

The Li-Chao Tree was introduced by **Li Chao** at ZJOI 2012. The Dynamic CHT is a classic competitive programming technique using balanced BSTs.

---
*Simplified and maintained for competitive programming and algorithm optimization.*
