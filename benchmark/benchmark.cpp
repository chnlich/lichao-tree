#include <bits/stdc++.h>
#include "lict.hpp"
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
            llint res = lict.query(op.val1);
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
            llint res = cht.query(op.val1);
            if (res < 2e18) sum += res;
        }
    }
    return sum;
}

const int NUM_RUNS = 10;

double median_time(vector<double>& times) {
    sort(times.begin(), times.end());
    int n = times.size();
    if (n % 2 == 0) return (times[n/2 - 1] + times[n/2]) / 2.0;
    return times[n/2];
}

void benchmark(int n, string dist_name, const vector<Operation>& ops, vector<Result>& results) {
    cerr << "Running benchmark: N=" << n << ", Dist=" << dist_name << " (" << NUM_RUNS << " runs, median)" << endl;

    // Run Li Chao (Standard) - NUM_RUNS times, take median
    vector<double> lc_times;
    long long sum_lc = 0;
    for (int r = 0; r < NUM_RUNS; ++r) {
        auto start = high_resolution_clock::now();
        sum_lc = run_lichao(ops);
        auto end = high_resolution_clock::now();
        lc_times.push_back(duration_cast<milliseconds>(end - start).count());
    }
    results.push_back({"LICT", n, dist_name, median_time(lc_times), sum_lc});

    // Run Dynamic CHT - NUM_RUNS times, take median
    vector<double> cht_times;
    long long sum_cht = 0;
    for (int r = 0; r < NUM_RUNS; ++r) {
        auto start = high_resolution_clock::now();
        sum_cht = run_cht(ops);
        auto end = high_resolution_clock::now();
        cht_times.push_back(duration_cast<milliseconds>(end - start).count());
    }
    results.push_back({"Dynamic CHT", n, dist_name, median_time(cht_times), sum_cht});

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
    cout << "\n# Benchmark Results (Median of " << NUM_RUNS << " runs)\n\n";
    cout << "| N | Distribution | Algorithm | Time (ms) | Checksum |\n";
    cout << "|---|---|---|---|---|\n";
    for (const auto& res : results) {
        cout << "| " << res.n << " | " << res.distribution << " | " << res.name << " | " << res.time_ms << " | " << res.checksum << " |\n";
    }

    // Output CSV to file
    ofstream csv("benchmark_results.csv");
    csv << "N,Distribution,Algorithm,Time_ms,Checksum\n";
    for (const auto& res : results) {
        csv << res.n << "," << res.distribution << "," << res.name << "," << res.time_ms << "," << res.checksum << "\n";
    }
    csv.close();
    cerr << "Results saved to benchmark_results.csv" << endl;

    return 0;
}
