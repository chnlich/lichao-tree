#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <string>
#include <fstream>

#include "lichao.hpp"
#include "lichao_static.hpp"
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
    LC::LiChaoTree lct(-1e9 - 7, 1e9 + 7);
    long long sum = 0;
    for (const auto& op : ops) {
        if (op.type == ADD) {
            lct.add_line(op.val1, op.val2);
        } else {
            sum += lct.query(op.val1);
        }
    }
    return sum;
}

long long run_lichao_static(const vector<Operation>& ops) {
    LCS::LiChaoTreeStatic lct(-1e9 - 7, 1e9 + 7);
    // Estimate size to reserve memory and prevent reallocations during insert
    // A rough upper bound for Li Chao is O(N log C).
    // For N=200k, log C ~ 60, it can be large, but usually much less nodes are created.
    // Let's reserve conservatively based on N.
    lct.reserve(ops.size() * 2); 
    
    long long sum = 0;
    for (const auto& op : ops) {
        if (op.type == ADD) {
            lct.add_line(op.val1, op.val2);
        } else {
            sum += lct.query(op.val1);
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
            sum += cht.query(op.val1);
        }
    }
    return sum;
}

void benchmark(int n, string dist_name, vector<Operation> ops, vector<Result>& results) {
    cerr << "Running benchmark: N=" << n << ", Dist=" << dist_name << endl;

    // Run Li Chao (Dynamic)
    auto start = high_resolution_clock::now();
    long long sum_lc = run_lichao(ops);
    auto end = high_resolution_clock::now();
    double time_lc = duration_cast<milliseconds>(end - start).count();
    
    results.push_back({"Li-Chao (Dynamic)", n, dist_name, time_lc, sum_lc});

    // Run Li Chao (Static)
    start = high_resolution_clock::now();
    long long sum_lcs = run_lichao_static(ops);
    end = high_resolution_clock::now();
    double time_lcs = duration_cast<milliseconds>(end - start).count();

    results.push_back({"Li-Chao (Static)", n, dist_name, time_lcs, sum_lcs});

    // Run CHT
    start = high_resolution_clock::now();
    long long sum_cht = run_cht(ops);
    end = high_resolution_clock::now();
    double time_cht = duration_cast<milliseconds>(end - start).count();

    results.push_back({"Dynamic CHT", n, dist_name, time_cht, sum_cht});

    if (sum_lc != sum_cht || sum_lcs != sum_cht) {
        cerr << "ERROR: Checksum mismatch! LC: " << sum_lc << " LCS: " << sum_lcs << " CHT: " << sum_cht << endl;
    }
}

int main() {
    vector<int> sizes = {10000, 100000, 200000};
    Generator gen(42);
    vector<Result> results;

    for (int n : sizes) {
        benchmark(n, "Random", gen.generate_random(n), results);
        benchmark(n, "Monotonic K", gen.generate_monotonic_k(n), results);
        benchmark(n, "Monotonic X", gen.generate_monotonic_x(n), results);
    }

    // Output Markdown Table to stdout
    cout << "\n# Benchmark Results\n\n";
    cout << "| N | Distribution | Algorithm | Time (ms) | Checksum |\n";
    cout << "|---|---|---|---|---|\n";
    for (const auto& res : results) {
        cout << "| " << res.n << " | " << res.distribution << " | " << res.name << " | " << res.time_ms << " | " << res.checksum << " |\n";
    }

    // Output CSV to file
    ofstream csv("benchmark_results.csv");
    csv << "N,Distribution,Algorithm,Time_ms\n";
    for (const auto& res : results) {
        csv << res.n << "," << res.distribution << "," << res.name << "," << res.time_ms << "\n";
    }
    csv.close();
    cerr << "Results saved to benchmark_results.csv" << endl;

    return 0;
}
