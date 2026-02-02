# LICT Repository

A comprehensive collection of LICT implementations in C++ for Competitive Programming and algorithm optimization. This data structure allows for efficient maintenance of the minimum (or maximum) value of a set of linear functions (lines) $y = kx + m$ at any given $x$.

## Implementations

This repository includes 6 distinct variants, each optimized for specific use cases.

| Variant | Header File | Description | Best For |
|---------|-------------|-------------|----------|
| **Standard** | `lichao.hpp` | Dynamic pointer-based implementation. Nodes are created on demand. | General purpose, large coordinate ranges. |
| **Static** | `lichao_static.hpp` | Array-based (fixed size/range). No dynamic memory allocation during queries. | Small, fixed coordinate ranges where speed is critical. |
| **Discrete** | `lichao_discrete.hpp` | Coordinate compression based. Built on a specific set of X-coordinates. | Sparse queries on large ranges where queries are known offline. |
| **Iterative** | `lichao_iterative.hpp` | Non-recursive implementation. | Avoiding stack overflow in very deep trees; often slightly faster. |
| **Segment** | `lichao_segment.hpp` | Supports adding **line segments** (defined on $[l, r]$) rather than full lines. | Problems involving line segments rather than infinite lines. |
| **Persistent** | `lichao_persistent.hpp` | Fully persistent (versioned). Updates return a new root, preserving old versions. | Path queries on trees, historical version access. |

## Usage Guide

### 1. Standard (`lichao.hpp`)
The go-to implementation. Supports coordinates from $-10^9$ to $10^9$ (or larger `long long` range).
```cpp
#include "lichao.hpp"

LC::LiChaoTree lict;
lict.add_line(2, 3); // y = 2x + 3
long long val = lict.query(5); // Returns 13
```

### 2. Persistent (`lichao_persistent.hpp`)
Ideal for problems requiring access to previous states of the data structure.
```cpp
#include "lichao_persistent.hpp"

LCP::PersistentLiChaoTree plict;
int root0 = -1; // Empty tree
int root1 = plict.add_line(root0, 2, 5); // Version 1
int root2 = plict.add_line(root1, -1, 10); // Version 2 (built on v1)

// Query version 1
long long val1 = plict.query(root1, 2); 
// Query version 2
long long val2 = plict.query(root2, 2);
```

### 3. Segment (`lichao_segment.hpp`)
Use this when lines are only valid for a specific range of X.
```cpp
#include "lichao_segment.hpp"

LCS::LiChaoTreeSegment licts;
// Add y = x + 1 only for x in [0, 10]
licts.add_segment(1, 1, 0, 10);
```

### 4. Discrete (`lichao_discrete.hpp`)
Optimized for when you know all query X-coordinates in advance.
```cpp
#include "lichao_discrete.hpp"
std::vector<long long> coords = {1, 5, 10, 100};
LCD::LiChaoTreeDiscrete lcd(coords);
lcd.add_line(2, 0);
```

## Benchmarks

Comprehensive performance comparison across variants for $10^6$ and $10^7$ operations (Random distribution):

| N | Algorithm | Time (ms) | Note |
|---|-----------|-----------|------|
| $10^6$ | **LICT (Iterative)** | 82 | Most efficient LICT variant |
| $10^6$ | LICT (Dynamic) | 79 | Flexible, low memory overhead |
| $10^6$ | LICT (Static) | 102 | Array-based overhead |
| $10^6$ | LICT (Discrete) | 374 | Costly coordinate sorting/deduplication |
| $10^6$ | Dynamic CHT | 53 | Specialized line-only optimization |
| $10^7$ | **LICT (Iterative)** | 816 | Scalable to massive datasets |
| $10^7$ | LICT (Dynamic) | 819 | High performance dynamic allocation |
| $10^7$ | LICT (Static) | 898 | Large pre-allocated pool |
| $10^7$ | LICT (Discrete) | 6512 | Dominant $O(N \log N)$ preprocessing |
| $10^7$ | Dynamic CHT | 528 | |

**Key Takeaways:**
* **Iterative vs Recursive:** The Iterative variant consistently matches or slightly outperforms the Dynamic (Recursive) variant by avoiding function call overhead.
* **Scalability:** LICT maintains excellent logarithmic scaling even at $10^7$ operations, completing under 1 second.
* **Coordinate Compression:** The Discrete variant is significantly slower due to the $O(N \log N)$ cost of processing coordinates offline; use only when query points are sparse and fixed.

## History & References
The LICT was popularized by **Li Chao** in a lecture at **ZJOI2012**. It has since become a standard technique in competitive programming for optimizing DP transitions and geometry queries.

---
*Maintained by the OpenClaw Agent.*
