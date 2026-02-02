#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <string>
#include <fstream>
#include <algorithm>

#include "lichao.hpp"
#include "zkw_lichao.hpp"
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

// Global coordinates for fixed-range testing
vector<ll> global_coords;

long long run_lichao_fixed(const vector<Operation>& ops) {
    LC::LiChaoTree lict(global_coords.front(), global_coords.back());
    long long sum = 0;
    for (const auto& op : ops) {
        if (op.type == ADD) {
            lict.add_line(op.val1, op.val2);
        } else {
            sum += lict.query(op.val1);
        }
    }
    return sum;
}

long long run_zkw_lichao_fixed(const vector<Operation>& ops) {
    ZKW_LC::ZkwLiChaoTree lict(global_coords.size(), global_coords.front(), global_coords.back());
    lict.set_coords(global_coords);
    long long sum = 0;
    for (const auto& op : ops) {
        if (op.type == ADD) {
            lict.add_line(op.val1, op.val2);
        } else {
            int idx = lower_bound(global_coords.begin(), global_coords.end(), op.val1) - global_coords.begin();
            sum += lict.query_by_idx(idx);
        }
    }
    return sum;
}

void benchmark(int n, string dist_name, vector<Operation> ops, vector<Result>& results) {
    cerr << "Running benchmark: N=" << n << ", Dist=" << dist_name << endl;
    
    global_coords.clear();
    for (const auto& op : ops) {
        if (op.type == QUERY) global_coords.push_back(op.val1);
    }
    sort(global_coords.begin(), global_coords.end());
    global_coords.erase(unique(global_coords.begin(), global_coords.end()), global_coords.end());

    if (global_coords.empty()) {
        global_coords.push_back(-1e9);
        global_coords.push_back(1e9);
    }

    // Run Li-Chao (Dynamic)
    auto start = high_resolution_clock::now();
    long long sum_lc = run_lichao_fixed(ops);
    auto end = high_resolution_clock::now();
    double time_lc = duration_cast<milliseconds>(end - start).count();
    results.push_back({"Li-Chao (Dynamic)", n, dist_name, time_lc, sum_lc});

    // Run Zkw-style Li-Chao
    start = high_resolution_clock::now();
    long long sum_zkw = run_zkw_lichao_fixed(ops);
    end = high_resolution_clock::now();
    double time_zkw = duration_cast<milliseconds>(end - start).count();
    results.push_back({"Zkw-Li-Chao", n, dist_name, time_zkw, sum_zkw});

    if (sum_lc != sum_zkw) {
        cerr << "WARNING: Checksum mismatch! LC: " << sum_lc << " ZKW: " << sum_zkw << endl;
    }
}

int main() {
    int n = 1000000;
    Generator gen(42);
    vector<Result> results;

    benchmark(n, "Random", gen.generate_random(n), results);
    benchmark(n, "Monotonic K", gen.generate_monotonic_k(n), results);
    benchmark(n, "Monotonic X", gen.generate_monotonic_x(n), results);

    cout << "\n# Benchmark Results (N = " << n << ")\n\n";
    cout << "| Distribution | Algorithm | Time (ms) | Checksum |\n";
    cout << "|---|---|---|---|\n";
    for (const auto& res : results) {
        cout << "| " << res.distribution << " | " << res.name << " | " << res.time_ms << " | " << res.checksum << " |\n";
    }

    return 0;
}
