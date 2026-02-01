#ifndef GENERATOR_HPP
#define GENERATOR_HPP

#include <vector>
#include <random>
#include <algorithm>

using namespace std;

typedef long long ll;

enum OpType { ADD, QUERY };

struct Operation {
    OpType type;
    ll val1; // k for ADD, x for QUERY
    ll val2; // m for ADD, ignored for QUERY
};

class Generator {
    mt19937_64 rng;
    ll min_coord, max_coord;

public:
    Generator(ll seed, ll min_c = -1e9, ll max_c = 1e9) : rng(seed), min_coord(min_c), max_coord(max_c) {}

    ll random(ll l, ll r) {
        uniform_int_distribution<ll> dist(l, r);
        return dist(rng);
    }

    // Fully random operations
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

    // Monotonic slopes (increasing k)
    vector<Operation> generate_monotonic_k(int n, double query_ratio = 0.5) {
        vector<Operation> ops = generate_random(n, query_ratio);
        vector<ll> slopes;
        for (auto &op : ops) {
            if (op.type == ADD) slopes.push_back(op.val1);
        }
        sort(slopes.begin(), slopes.end());
        int s_idx = 0;
        for (auto &op : ops) {
            if (op.type == ADD) op.val1 = slopes[s_idx++];
        }
        return ops;
    }
    
    // Monotonic queries (increasing x)
    vector<Operation> generate_monotonic_x(int n, double query_ratio = 0.5) {
        vector<Operation> ops = generate_random(n, query_ratio);
        vector<ll> queries;
        for (auto &op : ops) {
            if (op.type == QUERY) queries.push_back(op.val1);
        }
        sort(queries.begin(), queries.end());
        int q_idx = 0;
        for (auto &op : ops) {
            if (op.type == QUERY) op.val1 = queries[q_idx++];
        }
        return ops;
    }
};

#endif
