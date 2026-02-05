# Li-Chao Tree Project - Agent Guidelines

## Project Structure

```
lichao-tree/
├── README.md                 # Project overview and usage
├── AGENTS.md                 # This file - project philosophy and guidelines
├── Makefile                  # Root makefile (delegates to benchmark/)
├── .gitignore               # Excludes binaries and generated files
├── implementation/          # Core data structure implementations
│   ├── lichao.hpp         # Standard LICT (pointer-based segment tree)
│   └── cht.hpp            # Dynamic CHT (balanced BST-based)
├── generator/               # Test data generation
│   └── generator.hpp      # Two test distributions only
├── benchmark/               # Performance benchmarking
│   ├── Makefile           # Standalone benchmark build
│   └── benchmark.cpp      # Unified benchmark for both implementations
└── doc/                     # Documentation
    ├── report.tex         # LaTeX paper
    └── report.pdf         # Compiled paper
```

## Core Philosophy

### 1. Code Simplification Principle
**After benchmarking finds the winner, delete the losers.**

This project originally had 8+ implementations:
- Standard, Static, Discrete, Iterative, Segment, Persistent, RB/Set, ZKW variants

**Current state:** Only 2 implementations remain:
1. **Standard LICT** - Classic pointer-based segment tree (most educational/clarity)
2. **Dynamic CHT** - Balanced BST baseline (fastest general-case implementation)

**Why:** Readers don't need to see every variant tried. Focus on the comparison that proves the point.

### 2. Minimal Test Cases
**Only test what matters for the core argument.**

Two distributions only:
1. **Random** - O(√N) lines on hull (typical case, favors CHT)
2. **All on Hull** - O(N) lines on hull (worst case, tests full hull maintenance)

**Not included:**
- Monotonic slopes/queries (handled by Deque CHT, irrelevant to general-case comparison)
- Similar slopes, clustered queries, etc. (add noise without insight)

### 3. Clear Separation of Concerns
- `implementation/` - Pure algorithm code, no I/O or testing
- `generator/` - Test data generation only
- `benchmark/` - Timing and measurement only
- `doc/` - Paper and documentation only

### 4. When Making Changes

**Before adding:**
- Does this help prove the core comparison (LICT vs Dynamic CHT)?
- Is this the minimal code needed?

**Before deleting:**
- Is this proven slower by benchmarks?
- Will the paper narrative still make sense?

**Benchmarking rule:**
- Run `make && make run` to verify correctness (checksums must match)
- Update `report.tex` tables if results change
- Commit with descriptive messages explaining the "why"

## Historical Context

The Li-Chao Tree was invented by Li Chao at ZJOI 2012 for online line envelope maintenance. While widely used for DP optimization, that is an application — not the original purpose.

**Key insight from benchmarks:**
- Dynamic CHT is consistently ~1.4-1.6× faster
- But LICT offers simpler implementation (only integer comparisons)
- Both scale linearly with problem size as expected

**Trade-off:** Speed vs. Simplicity. The paper presents both sides.

## Git Workflow

```bash
# Build and verify
make clean && make
./benchmark/benchmark_runner

# Commit changes
git add -A
git commit -m "descriptive message"
```

## Files Not to Touch

- `generator/generator.hpp` - Keep only 2 test cases (random + all_on_hull)
- `implementation/` - Only Standard LICT and Dynamic CHT
- No adding back deleted variants without strong justification

## Coding Preferences

### Type Aliases
Use `llint` (not `ll`) for `long long`:
```cpp
typedef long long llint;
```

### Includes
Use `<bits/stdc++.h>` for competitive programming style:
```cpp
#include <bits/stdc++.h>
```

This is preferred over explicit individual headers for simplicity and faster compilation in CP contexts.

### Naming Conventions
- `llint` for 64-bit integers
- `INF` for infinity constants (uppercase)
- Classes in `PascalCase`
- Methods in `snake_case`
- Member variables: use trailing underscore or clear naming (avoid `m_` prefix)

## Paper Writing Preferences

### Tone and Style
- **Academic and formal**: Use strong academic language, avoid overly narrative or "humble" tones
- **Objective framing**: Present LICT as an alternative approach with its own advantages, not as inferior/outperformed
- **No contest references**: Don't mention specific contests like "ZJOI 2012", just say "2012"

### Typography
- **No dashes**: Replace `--` (en-dash) with "to" or rephrase (e.g., "1.5 to 2.0 times" instead of "1.5--2.0×")
- Use proper mathematical notation: `$1.5$ to $2.0\times$`

### Content Structure
- **Introduction**: Keep benchmark details (sizes, specific numbers) out of the introduction
- **Abstract**: Frame LICT as a viable alternative with distinct advantages (simplicity, persistency, extensibility)
- **Comparison**: Highlight persistency, line segment support, and implementation advantages as key differentiators
- **Conclusion**: Emphasize LICT's unique position in the design space, not as second-best

### Paper Organization
Papers should follow this structure:
1. **Abstract** - Summary of contributions and key findings
2. **Introduction** - Problem definition, background, motivation, contributions
3. **Related Work** - Prior approaches and their limitations
4. **Overview** - High-level description of the approach, key trade-offs, summary of results (before diving into details)
5. **Detailed Sections** - Implementation details, benchmarks, analysis (at minimum: Implementation and Benchmark sections)
6. **Conclusion** - Summary of findings and future directions

The Overview section serves as a bridge between Related Work and the detailed technical sections, giving readers a high-level understanding before presenting code and empirical results.
