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
    vector<Operation> generate_all_on_hull(int n, double query_ratio = 0.5) {
        vector<Operation> ops;
        ops.reserve(n);
        
        for (int i = 0; i < n; ++i) {
            if (uniform_real_distribution<>(0, 1)(rng) < query_ratio && i > 0) {
                llint x = random(max_coord / 4, max_coord / 2);
                ops.push_back({QUERY, x, 0});
            } else {
                llint k = -(i + 1);
                llint m = (llint)(i + 1) * (i + 1);
                ops.push_back({ADD, k, m});
            }
        }
        return ops;
    }
};

#endif
