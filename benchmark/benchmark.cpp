#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <string>
#include <fstream>

#include "lichao.hpp"
#include "cht.hpp"
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

long long run_lichao(const vector<Operation>& ops) {
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

long long run_cht(const vector<Operation>& ops) {
    CHT::DynamicCHT cht;
    long long sum = 0;
    for (const auto& op : ops) {
        if (op.type == ADD) {
            cht.add_line(op.val1, op.val2);
        } else {
            ll res = cht.query(op.val1);
            if (res < 2e18) sum += res;
        }
    }
    return sum;
}

void benchmark(int n, string dist_name, vector<Operation> ops, vector<Result>& results) {
    cerr << "Running benchmark: N=" << n << ", Dist=" << dist_name << endl;

    // Run Li Chao (Standard)
    auto start = high_resolution_clock::now();
    long long sum_lc = run_lichao(ops);
    auto end = high_resolution_clock::now();
    double time_lc = duration_cast<milliseconds>(end - start).count();
    results.push_back({"LICT (Standard)", n, dist_name, time_lc, sum_lc});

    // Run Dynamic CHT
    start = high_resolution_clock::now();
    long long sum_cht = run_cht(ops);
    end = high_resolution_clock::now();
    double time_cht = duration_cast<milliseconds>(end - start).count();
    results.push_back({"Dynamic CHT", n, dist_name, time_cht, sum_cht});

    if (sum_lc != sum_cht) {
        cerr << "WARNING: Checksum mismatch! LICT: " << sum_lc << " CHT: " << sum_cht << endl;
    }
}

int main() {
    vector<int> sizes = {100000, 1000000, 10000000};
    Generator gen(42);
    vector<Result> results;

    for (int n : sizes) {
        benchmark(n, "Random", gen.generate_random(n), results);
        benchmark(n, "All on Hull", gen.generate_all_on_hull(n), results);
    }

    // Output Markdown Table to stdout
    cout << "\n# Benchmark Results\n\n";
    cout << "| N | Distribution | Algorithm | Time (ms) | Checksum |\n";
    cout << "|---|---|---|---|---|\n";
    for (const auto& res : results) {
        cout << "| " << res.n << " | " << res.distribution << " | " << res.name << " | " << res.time_ms << " | " << res.checksum << " |\n";
    }

    // Output CSV to file
    ofstream csv("benchmark/benchmark_results.csv");
    csv << "N,Distribution,Algorithm,Time_ms,Checksum\n";
    for (const auto& res : results) {
        csv << res.n << "," << res.distribution << "," << res.name << "," << res.time_ms << "," << res.checksum << "\n";
    }
    csv.close();
    cerr << "Results saved to benchmark_results.csv" << endl;

    return 0;
}
