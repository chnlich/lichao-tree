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
- **NO DASHES ALLOWED**: Never use `--` (en-dash) in the paper. Always replace with "to" or rephrase:
  - ❌ Wrong: `1.45--2.00 times`, `time---$O(\log C)$---to`, `midpoint---never both`
  - ✅ Correct: `1.45 to 2.00 times`, `time $O(\log C)$ to`, `midpoint, never both`
- Use proper mathematical notation: `$1.5$ to $2.0\times$`
- This is a strict rule - check for `--` before every commit

### Additional rules
- **No speed comparisons in abstract** - Only describe what LICT achieves, not relative performance vs CHT
- **No "Selection Guidelines" subsection** - Remove selection guidance from abstract and delete from Discussion
- **Simplify complexity statements** - State only LICT's $O(\log C)$ without comparing to CHT's $O(\log n)$
- **Do NOT mention specific competing solutions in Introduction** - No CHT framing in Intro, comparison belongs in Related Work
- **$N$ is the query size** - using uppercase N instead of n in the paper.
- **$C$ is a precision parameter** - $C = \frac{\text{coordinate range}}{\text{precision level}}$
- **Line segment complexity is $O(\log^2 C)$** - Not $O(\log C)$ (segment decomposes into $O(\log C)$ intervals)
- **LICT uses only $kx+b$ evaluations** - Emphasize avoidance of division precision issues
- **Don't claim unverified applications** - Only state "extensive applications" without listing specific domains
- **No performance ratios** - Don't state "X times faster"; use neutral descriptions ("faster execution")
- **Merge related sections** - Consolidate sections to avoid redundancy
- **Don't mention Segment Tree Beats** - Unrelated to Li-Chao tree
- **Don't mention Kinetic Data Structures** - Unrelated to Li-Chao tree
- **Remove obvious sections** - Don't include trivial implementation details (Empty Container Behavior, Node reclamation, etc.)

### Math Expression Formatting
- **Displayed equations**: Each complex math expression should occupy its own line using `\[` and `\]` (not inline `$...$`)
- **Simple expressions**: Simple inline math like `$O(\log C)$`, `$k_i$`, `$x$` can remain inline
- **Consistency**: Use consistent symbols throughout: `l, r, m` (not `left, right, mid`)

### Paper Organization
Papers should follow this structure:
1. **Abstract** - Summary of contributions and key findings
2. **Introduction** - Problem definition, background, motivation, contributions
3. **Related Work** - Prior approaches and their limitations (e.g., CHT variants)
4. **Main Contribution Section** - The paper's primary contribution (e.g., "The Li-Chao Tree") with algorithmic approach, comparison with prior work, trade-offs, and summary of results
5. **Detailed Sections** - Implementation details, benchmarks, analysis (at minimum: Implementation and Benchmark sections)
6. **Discussion** - Analysis of advantages, selection guidelines
7. **Conclusion** - Summary of findings

The main contribution section should come AFTER Related Work and BEFORE the detailed Implementation/Benchmarks sections. Related Work should only cover prior approaches, not the paper's own contribution.
