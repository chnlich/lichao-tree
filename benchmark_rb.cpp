#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <string>
#include <fstream>

#include "lichao.hpp"
#include "zkw_lichao.hpp"
#include "rb_lichao.hpp"
#include "generator.hpp"

using namespace std;
using namespace std::chrono;

struct Result {
    string name;
    int n;
    string distribution;
    double time_ms;
    long long checksum;
};

long long run_lichao_dynamic(const vector<Operation>& ops) {
    LC::LiChaoTree lict(-1e9 - 7, 1e9 + 7);
    long long sum = 0;
    for (const auto& op : ops) {
        if (op.type == ADD) {
            lict.add_line(op.val1, op.val2);
        } else {
            ll res = lict.query(op.val1);
            if (res < 2e18) sum += res;
        }
    }
    return sum;
}

long long run_lichao_zkw(const vector<Operation>& ops) {
    // 1. Collect and sort coords for ZKW
    vector<long long> coords;
    coords.reserve(ops.size());
    for (const auto& op : ops) {
        if (op.type == QUERY) {
            coords.push_back(op.val1);
        }
    }
    sort(coords.begin(), coords.end());
    coords.erase(unique(coords.begin(), coords.end()), coords.end());

    ZKW_LC::ZkwLiChaoTree lict(coords.size(), -1e9 - 7, 1e9 + 7);
    lict.set_coords(coords);
    
    long long sum = 0;
    for (const auto& op : ops) {
        if (op.type == ADD) {
            lict.add_line(op.val1, op.val2);
        } else {
            ll res = lict.query(op.val1);
            if (res < 2e18) sum += res;
        }
    }
    return sum;
}

long long run_lichao_rb(const vector<Operation>& ops) {
    RB_LC::RBLiChaoTree lict;
    long long sum = 0;
    for (const auto& op : ops) {
        if (op.type == ADD) {
            lict.add_line(op.val1, op.val2);
        } else {
            ll res = lict.query(op.val1);
            if (res < 2e18) sum += res;
        }
    }
    return sum;
}

void benchmark(int n, string dist_name, vector<Operation> ops, vector<Result>& results) {
    cerr << "Running benchmark: N=" << n << ", Dist=" << dist_name << endl;

    // Run Li Chao (Dynamic)
    auto start = high_resolution_clock::now();
    long long sum_dyn = run_lichao_dynamic(ops);
    auto end = high_resolution_clock::now();
    double time_dyn = duration_cast<milliseconds>(end - start).count();
    results.push_back({"Li-Chao (Dynamic)", n, dist_name, time_dyn, sum_dyn});

    // Run Li Chao (ZKW)
    start = high_resolution_clock::now();
    long long sum_zkw = run_lichao_zkw(ops);
    end = high_resolution_clock::now();
    double time_zkw = duration_cast<milliseconds>(end - start).count();
    results.push_back({"Li-Chao (ZKW)", n, dist_name, time_zkw, sum_zkw});

    // Run Li Chao (RB)
    start = high_resolution_clock::now();
    long long sum_rb = run_lichao_rb(ops);
    end = high_resolution_clock::now();
    double time_rb = duration_cast<milliseconds>(end - start).count();
    results.push_back({"Li-Chao (RB/Set)", n, dist_name, time_rb, sum_rb});

    if (sum_dyn != sum_zkw || sum_dyn != sum_rb) {
        cerr << "WARNING: Checksum mismatch! DYN: " << sum_dyn << " ZKW: " << sum_zkw << " RB: " << sum_rb << endl;
    }
}

int main() {
    vector<int> sizes = {1000000};
    Generator gen(42);
    vector<Result> results;

    for (int n : sizes) {
        benchmark(n, "Random", gen.generate_random(n), results);
        benchmark(n, "Monotonic K", gen.generate_monotonic_k(n), results);
        benchmark(n, "Monotonic X", gen.generate_monotonic_x(n), results);
        benchmark(n, "All on Hull", gen.generate_all_on_hull(n), results);
        benchmark(n, "Similar Slopes", gen.generate_similar_slopes(n), results);
    }

    cout << "\n# Benchmark Results (N=1,000,000)\n\n";
    cout << "| Distribution | Algorithm | Time (ms) | Checksum |\n";
    cout << "|---|---|---|---|\n";
    for (const auto& res : results) {
        cout << "| " << res.distribution << " | " << res.name << " | " << res.time_ms << " | " << res.checksum << " |\n";
    }

    return 0;
}
