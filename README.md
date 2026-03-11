# Li-Chao Tree (LICT): Reference Implementation

Reference implementation accompanying the paper *The Li-Chao Tree: Algorithm Specification and Analysis* ([doc/report.pdf](doc/report.pdf)).

## Overview

This repository provides optimized C++ implementations for maintaining the lower envelope of a set of linear functions (lines) $y = kx + m$, supporting efficient line insertion and point query operations. It includes the standard LICT, a line segment variant, an iterative (ZKW) variant optimized for the $N = C$ regime, and a Dynamic Convex Hull Trick baseline.

## Implementations

| Implementation | Header File | Description | Time Complexity |
|----------------|-------------|-------------|-----------------|
| **LICT (Standard)** | `lict.hpp` | Classic pointer-based segment tree implementation | $O(\log C)$ insertion/query |
| **Segment LICT** | `segment_lict.hpp` | Line segment LICT with interval decomposition | $O(\log^2 C)$ insertion, $O(\log C)$ query |
| **ZKW LICT** | `zkw_lict.hpp` | Iterative (ZKW) LICT variant, better cache locality for $N = C$ regime | $O(\log C)$ insertion/query |
| **Dynamic CHT** | `cht.hpp` | Classic Convex Hull Trick with balanced BST | $O(\log n)$ insertion/query |

## Structure

```
lichao-tree/
├── README.md                    - This file
├── AGENTS.md                    - Worker instructions
├── CLAUDE.md                    - Symlink to AGENTS.md
├── Makefile                     - Root makefile (delegates to benchmark/)
├── .gitignore                   - Git ignore rules
├── arxiv.tar.gz                 - arXiv submission archive (gitignored)
├── implementation/
│   ├── lict.hpp                 - Standard Li-Chao Tree
│   ├── segment_lict.hpp         - Line segment LICT
│   ├── zkw_lict.hpp             - Iterative (ZKW) LICT
│   └── cht.hpp                  - Dynamic CHT implementation
├── generator/
│   └── generator.hpp            - Test data generator
├── benchmark/
│   ├── Makefile                 - Benchmark-specific makefile
│   ├── benchmark.cpp            - Benchmark source
│   └── benchmark_results.csv    - Generated results
└── doc/
    ├── report.tex               - LaTeX report
    ├── report.pdf               - Compiled PDF
    └── references.bib           - Bibliography
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

Or manually in the benchmark folder:

```bash
cd benchmark
make
./benchmark_runner
```

### Using LICT (Standard)

```cpp
#include "implementation/lict.hpp"

LC::LiChaoTree lict(-1e9, 1e9);  // Create with coordinate range
lict.add_line(2, 3);              // Add line y = 2x + 3
long long val = lict.query(5);    // Query at x = 5, returns 13
```

### Using Segment LICT

```cpp
#include "implementation/segment_lict.hpp"

LC_Segment::LiChaoTree lict(-1e9, 1e9);  // Create with coordinate range
lict.add_line(2, 3);                      // Add global line y = 2x + 3
lict.add_segment(1, 5, 0, 100);           // Add segment y = x + 5 on [0, 100]
long long val = lict.query(50);            // Query at x = 50
```

### Using ZKW LICT

```cpp
#include "implementation/zkw_lict.hpp"

LC_ZKW::LiChaoTree lict(0, 1000000);  // Create with coordinate range (must be fixed)
lict.add_line(2, 3);                   // Add line y = 2x + 3
long long val = lict.query(5);         // Query at x = 5, returns 13
```

### Using Dynamic CHT

```cpp
#include "implementation/cht.hpp"

CHT::DynamicCHT cht;
cht.add_line(2, 3);              // Add line y = 2x + 3
long long val = cht.query(5);    // Query at x = 5, returns 13
```

## Benchmarks

The benchmark compares implementations across:
- **N values:** $10^5$, $10^6$, $10^7$ operations
- **Distributions:**
  - **Random:** Random slopes and intercepts (expected hull size $\Theta(\log N)$)
  - **All on Hull:** Lines constructed so all $N/2$ contribute to the lower envelope

All times are averages over 10 runs. See [doc/report.pdf](doc/report.pdf) for full experimental setup.

### Table 1: General Performance (Time in milliseconds)

| N | Distribution | Algorithm | Insert (ms) | Query (ms) | Total (ms) |
|---|--------------|-----------|-------------|------------|------------|
| $10^5$ | Random | LICT | 4.20 | 0.78 | 4.98 |
| $10^5$ | Random | Dynamic CHT | 2.21 | 0.29 | 2.50 |
| $10^5$ | All on Hull | LICT | 9.17 | 7.25 | 16.42 |
| $10^5$ | All on Hull | Dynamic CHT | 7.88 | 7.00 | 14.88 |
| $10^6$ | Random | LICT | 41.63 | 7.84 | 49.48 |
| $10^6$ | Random | Dynamic CHT | 22.74 | 2.80 | 25.53 |
| $10^6$ | All on Hull | LICT | 245.17 | 248.77 | 493.94 |
| $10^6$ | All on Hull | Dynamic CHT | 294.86 | 304.05 | 598.91 |
| $10^7$ | Random | LICT | 431.80 | 78.88 | 510.68 |
| $10^7$ | Random | Dynamic CHT | 226.64 | 33.48 | 260.12 |
| $10^7$ | All on Hull | LICT | 6643.19 | 7045.61 | 13688.80 |
| $10^7$ | All on Hull | Dynamic CHT | 7558.58 | 7417.51 | 14976.10 |

### Table 2: N = C Regime (Time in milliseconds)

| N = C | Distribution | Algorithm | Insert (ms) | Query (ms) | Total (ms) |
|-------|--------------|-----------|-------------|------------|------------|
| $10^5$ | Random | LICT | 2.21 | 0.94 | 3.16 |
| $10^5$ | Random | ZKW LICT | 2.40 | 0.58 | 2.98 |
| $10^5$ | Random | Dynamic CHT | 2.23 | 0.41 | 2.64 |
| $10^5$ | All on Hull | LICT | 7.11 | 5.42 | 12.54 |
| $10^5$ | All on Hull | ZKW LICT | 5.32 | 1.70 | 7.02 |
| $10^5$ | All on Hull | Dynamic CHT | 7.17 | 6.83 | 14.00 |
| $10^6$ | Random | LICT | 26.54 | 9.46 | 36.00 |
| $10^6$ | Random | ZKW LICT | 27.52 | 6.59 | 34.11 |
| $10^6$ | Random | Dynamic CHT | 23.57 | 3.77 | 27.33 |
| $10^6$ | All on Hull | LICT | 127.28 | 177.07 | 304.35 |
| $10^6$ | All on Hull | ZKW LICT | 71.79 | 41.68 | 113.47 |
| $10^6$ | All on Hull | Dynamic CHT | 187.77 | 260.54 | 448.31 |
| $10^7$ | Random | LICT | 311.44 | 99.31 | 410.75 |
| $10^7$ | Random | ZKW LICT | 324.54 | 77.98 | 402.51 |
| $10^7$ | Random | Dynamic CHT | 235.24 | 45.01 | 280.25 |
| $10^7$ | All on Hull | LICT | 4022.59 | 5248.88 | 9271.48 |
| $10^7$ | All on Hull | ZKW LICT | 1685.15 | 1428.63 | 3113.78 |
| $10^7$ | All on Hull | Dynamic CHT | 6163.38 | 6579.40 | 12742.78 |

## When to Use Which?

- **Standard LICT** (`lict.hpp`): General purpose, simplest implementation. Use when you need reliability, integer arithmetic (no floating point), and easy extensibility to persistence or segments. Best default choice.

- **Segment LICT** (`segment_lict.hpp`): Use for line segment problems where lines are valid only on subranges $[x_l, x_r]$. Supports both global lines and segments. $O(\log^2 C)$ per segment insertion.

- **ZKW LICT** (`zkw_lict.hpp`): Iterative, array-based variant optimized for DP problems where $N = C$. Eliminates recursion overhead and uses contiguous memory for better cache locality. Approximately 4x faster than Dynamic CHT on all-on-hull workloads at $N = C = 10^7$ (3113 ms vs 12742 ms).

- **Dynamic CHT** (`cht.hpp`): Maximum raw performance on random distributions where few lines remain on the hull. $O(\log N)$ complexity means it outperforms LICT when the hull is small relative to the coordinate range. Best choice when hull sparsity is expected.

## Technical Details

### Standard LICT
The Standard Li-Chao Tree is a binary segment tree where each node stores the line that is optimal at the midpoint of its interval. Insertion and query both take $O(\log C)$ time where $C$ is the coordinate range. Uses simple integer comparisons with no floating point arithmetic.

### Segment LICT
Extends the standard LICT to support line segments defined on finite intervals $[x_l, x_r]$. The segment is decomposed into $O(\log C)$ canonical intervals of the implicit segment tree, and a standard LICT insertion is performed on each. Total cost per segment insertion is $O(\log^2 C)$. Also supports global line insertion at $O(\log C)$.

### ZKW LICT
An iterative (non-recursive) LICT variant using a flat array layout, inspired by ZKW segment trees. Requires the coordinate range to be fixed at construction. The contiguous memory layout improves cache performance, and the iterative traversal eliminates function call overhead. Particularly effective in the $N = C$ regime common in DP optimizations.

### Dynamic CHT
The Dynamic CHT uses a balanced BST (`std::multiset`) to maintain the convex hull explicitly. Insertion and query take $O(\log n)$ time where $n$ is the number of lines on the hull. Requires computing intersection points between adjacent lines, which introduces floating point considerations for near-parallel lines.

## History

The Li-Chao Tree algorithm was introduced by Li Chao (Chao Li) in lecture materials at ZJOI 2012. Despite widespread adoption in the competitive programming community, no formal specification had appeared in the literature until the accompanying paper ([doc/report.pdf](doc/report.pdf)), which provides the definitive formalization, correctness proofs, complexity analysis, and empirical benchmarks. The Dynamic CHT is a classic competitive programming technique based on the Convex Hull Trick with balanced BSTs.

---
*Reference implementation for "The Li-Chao Tree: Algorithm Specification and Analysis." See [doc/report.pdf](doc/report.pdf) for the full paper.*
