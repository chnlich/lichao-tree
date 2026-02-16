#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include <bits/stdc++.h>

using namespace std;

typedef long long llint;

enum OpType { ADD, QUERY };

struct Operation {
    OpType type;
    llint val1; // k for ADD, x for QUERY
    llint val2; // m for ADD, ignored for QUERY
};

class Generator {
    mt19937_64 rng;
    llint min_coord, max_coord;

public:
    Generator(llint seed, llint min_c = -1e9, llint max_c = 1e9) : rng(seed), min_coord(min_c), max_coord(max_c) {}

    llint random(llint l, llint r) {
        uniform_int_distribution<llint> dist(l, r);
        return dist(rng);
    }

    // Random operations: O(sqrt(N)) lines on hull
    vector<Operation> generate_random(int n, double query_ratio = 0.5) {
        vector<Operation> ops;
        ops.reserve(n);
        for (int i = 0; i < n; ++i) {
            if (uniform_real_distribution<>(0, 1)(rng) < query_ratio && !ops.empty()) {
                ops.push_back({QUERY, random(min_coord, max_coord), 0});
            } else {
                ops.push_back({ADD, random(min_coord, max_coord), random(min_coord, max_coord)});
            }
        }
        return ops;
    }

    // All lines on hull: y = -i*x + i^2, all tangent to parabola y = -x^2/4
    // For minimum queries, all N lines contribute to the lower envelope.
    // Adjacent lines intersect at x = 2i+1, so queries must cover [0, ~2N].
    vector<Operation> generate_all_on_hull(int n, double query_ratio = 0.5) {
        vector<Operation> ops;
        ops.reserve(n);
        
        // First, generate all insert operations
        vector<pair<llint,llint>> lines;
        int num_inserts = 0;
        int num_queries = 0;
        for (int i = 0; i < n; ++i) {
            if (uniform_real_distribution<>(0, 1)(rng) < query_ratio && i > 0) {
                num_queries++;
            } else {
                llint k = -(num_inserts + 1);
                llint m = (llint)(num_inserts + 1) * (num_inserts + 1);
                lines.push_back({k, m});
                num_inserts++;
            }
        }
        
        // Shuffle to avoid monotonic slope insertion order
        shuffle(lines.begin(), lines.end(), rng);
        
        // Interleave shuffled inserts and queries covering intersection region
        int ins_idx = 0;
        int q_count = 0;
        // Reset RNG state for reproducible query generation
        mt19937_64 qrng(rng);
        for (int i = 0; i < n; ++i) {
            if (q_count < num_queries && ins_idx > 0 && 
                uniform_real_distribution<>(0, 1)(qrng) < query_ratio) {
                // Query in [0, 2*num_inserts] to cover intersection points
                llint x = uniform_int_distribution<llint>(0, (llint)2 * num_inserts)(qrng);
                ops.push_back({QUERY, x, 0});
                q_count++;
            } else if (ins_idx < (int)lines.size()) {
                ops.push_back({ADD, lines[ins_idx].first, lines[ins_idx].second});
                ins_idx++;
            } else {
                llint x = uniform_int_distribution<llint>(0, (llint)2 * num_inserts)(qrng);
                ops.push_back({QUERY, x, 0});
                q_count++;
            }
        }
        return ops;
    }
};

#endif
