#include <bits/stdc++.h>
#include "lict.hpp"
#include "zkw_lict.hpp"
#include "cht.hpp"
#include "generator.hpp"

using namespace std;
using namespace std::chrono;

// ============================================================================
// STANDARD BENCHMARK
// ============================================================================

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

double average_time(const vector<double>& times) {
    double sum = 0;
    for (double t : times) sum += t;
    return sum / times.size();
}

void benchmark(int n, string dist_name, const vector<Operation>& ops, vector<Result>& results) {
    cerr << "Running benchmark: N=" << n << ", Dist=" << dist_name << " (" << NUM_RUNS << " runs, average)" << endl;

    // Run Li Chao (Standard) - NUM_RUNS times, take average
    vector<double> lc_times;
    long long sum_lc = 0;
    for (int r = 0; r < NUM_RUNS; ++r) {
        auto start = high_resolution_clock::now();
        sum_lc = run_lichao(ops);
        auto end = high_resolution_clock::now();
        lc_times.push_back(duration_cast<milliseconds>(end - start).count());
    }
    results.push_back({"LICT", n, dist_name, average_time(lc_times), sum_lc});

    // Run Dynamic CHT - NUM_RUNS times, take average
    vector<double> cht_times;
    long long sum_cht = 0;
    for (int r = 0; r < NUM_RUNS; ++r) {
        auto start = high_resolution_clock::now();
        sum_cht = run_cht(ops);
        auto end = high_resolution_clock::now();
        cht_times.push_back(duration_cast<milliseconds>(end - start).count());
    }
    results.push_back({"Dynamic CHT", n, dist_name, average_time(cht_times), sum_cht});

    if (sum_lc != sum_cht) {
        cerr << "WARNING: Checksum mismatch! LICT: " << sum_lc << " CHT: " << sum_cht << endl;
    }
}

void run_n_smaller_than_c() {
    cout << "=================================================================" << endl;
    cout << "BENCHMARK: N < C Regime (Standard)" << endl;
    cout << "=================================================================" << endl;

    vector<int> sizes = {100000, 1000000, 10000000};
    Generator gen(42);
    vector<Result> results;

    for (int n : sizes) {
        benchmark(n, "Random", gen.generate_random(n), results);
        benchmark(n, "All on Hull", gen.generate_all_on_hull(n), results);
    }

    // Output Markdown Table to stdout
    cout << "\n# Benchmark Results (Average of " << NUM_RUNS << " runs)\n\n";
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
}

// ============================================================================
// EXPERIMENTS (N = C Regime, ZKW Comparison)
// ============================================================================

struct NCResult {
    int n;
    int c;
    string distribution;
    string algorithm;
    double insert_time_ms;
    double query_time_ms;
    double total_time_ms;
    long long checksum;
};

NCResult run_nc_lichao(const vector<pair<llint,llint>>& inserts, const vector<llint>& queries, llint max_range) {
    NCResult res;
    LC::LiChaoTree lict(-max_range - 1, max_range + 1);
    
    // Time insertions
    auto start = high_resolution_clock::now();
    for (const auto& ins : inserts) {
        lict.add_line(ins.first, ins.second);
    }
    auto mid = high_resolution_clock::now();
    
    // Time queries
    long long sum = 0;
    for (llint x : queries) {
        llint val = lict.query(x);
        if (val < 4e18) sum += val;
    }
    auto end = high_resolution_clock::now();
    
    res.insert_time_ms = duration_cast<microseconds>(mid - start).count() / 1000.0;
    res.query_time_ms = duration_cast<microseconds>(end - mid).count() / 1000.0;
    res.total_time_ms = duration_cast<microseconds>(end - start).count() / 1000.0;
    res.checksum = sum;
    res.algorithm = "LICT";
    
    return res;
}

NCResult run_nc_zkw(const vector<pair<llint,llint>>& inserts, const vector<llint>& queries, llint max_range) {
    NCResult res;
    LC_ZKW::LiChaoTree lict(-max_range - 1, max_range + 1);
    
    // Time insertions
    auto start = high_resolution_clock::now();
    for (const auto& ins : inserts) {
        lict.add_line(ins.first, ins.second);
    }
    auto mid = high_resolution_clock::now();
    
    // Time queries
    long long sum = 0;
    for (llint x : queries) {
        llint val = lict.query(x);
        if (val < 4e18) sum += val;
    }
    auto end = high_resolution_clock::now();
    
    res.algorithm = "ZKW LICT";
    res.insert_time_ms = duration_cast<microseconds>(mid - start).count() / 1000.0;
    res.query_time_ms = duration_cast<microseconds>(end - mid).count() / 1000.0;
    res.total_time_ms = duration_cast<microseconds>(end - start).count() / 1000.0;
    res.checksum = sum;
    
    return res;
}

NCResult run_nc_cht(const vector<pair<llint,llint>>& inserts, const vector<llint>& queries, llint) {
    NCResult res;
    CHT::DynamicCHT cht;
    
    // Time insertions
    auto start = high_resolution_clock::now();
    for (const auto& ins : inserts) {
        cht.add_line(ins.first, ins.second);
    }
    auto mid = high_resolution_clock::now();
    
    // Time queries
    long long sum = 0;
    for (llint x : queries) {
        llint val = cht.query(x);
        if (val < 4e18) sum += val;
    }
    auto end = high_resolution_clock::now();
    
    res.insert_time_ms = duration_cast<microseconds>(mid - start).count() / 1000.0;
    res.query_time_ms = duration_cast<microseconds>(end - mid).count() / 1000.0;
    res.total_time_ms = duration_cast<microseconds>(end - start).count() / 1000.0;
    res.checksum = sum;
    res.algorithm = "Dynamic CHT";
    
    return res;
}

void run_n_equals_c() {
    cout << "\n=================================================================" << endl;
    cout << "BENCHMARK: N = C Regime" << endl;
    cout << "=================================================================\n" << endl;
    
    vector<NCResult> results;
    
    vector<pair<int, llint>> test_cases = {
        {100000, 100000}, 
        {1000000, 1000000}, 
        {10000000, 10000000},

    };
    vector<string> distributions = {"Random", "All on Hull"};
    
    for (const auto& tc : test_cases) {
        int n = tc.first;
        llint c = tc.second;
        
        for (const auto& dist : distributions) {
            cout << "Testing N=" << n << ", C=" << c << ", Dist=" << dist << "..." << endl;
            
            Generator gen(42);
            auto [inserts, queries] = (dist == "Random")
                ? gen.generate_nc(n, c)
                : gen.generate_nc_all_on_hull(n, c);
            
            // Compute range that covers all query points
            llint max_range = c / 2;
            for (auto x : queries) max_range = max(max_range, abs(x));
            
            // Run LICT
            auto res_lichao = run_nc_lichao(inserts, queries, max_range);
            res_lichao.n = n;
            res_lichao.c = c;
            res_lichao.distribution = dist;
            results.push_back(res_lichao);
            
            // Run ZKW LICT
            auto res_zkw = run_nc_zkw(inserts, queries, max_range);
            res_zkw.n = n;
            res_zkw.c = c;
            res_zkw.distribution = dist;
            results.push_back(res_zkw);
            
            // Run CHT
            auto res_cht = run_nc_cht(inserts, queries, c);
            res_cht.n = n;
            res_cht.c = c;
            res_cht.distribution = dist;
            results.push_back(res_cht);
            
            // Verify checksums match
            if (res_lichao.checksum != res_cht.checksum) {
                cerr << "WARNING: Checksum mismatch! LICT: " << res_lichao.checksum 
                     << " CHT: " << res_cht.checksum << endl;
            }
            if (res_lichao.checksum != res_zkw.checksum) {
                cerr << "WARNING: Checksum mismatch! LICT: " << res_lichao.checksum
                     << " ZKW: " << res_zkw.checksum << endl;
            }
        }
    }
    
    // Output Markdown Table
    cout << "\n## Experiment 1: N = C Regime\n" << endl;
    cout << "| N = C | Distribution | Algorithm | Insert Time (ms) | Query Time (ms) | Total Time (ms) |" << endl;
    cout << "|-------|-------------|-----------|------------------|-----------------|-----------------|" << endl;
    for (const auto& r : results) {
        cout << "| " << r.n << " | " << r.distribution << " | " << r.algorithm << " | " 
             << fixed << setprecision(2) << r.insert_time_ms << " | "
             << r.query_time_ms << " | " << r.total_time_ms << " |" << endl;
    }
    
    // Save to CSV
    ofstream csv1("experiment1_nc_results.csv");
    csv1 << "N,C,Distribution,Algorithm,Insert_Time_ms,Query_Time_ms,Total_Time_ms,Checksum\n";
    for (const auto& r : results) {
        csv1 << r.n << "," << r.c << "," << r.distribution << "," << r.algorithm << ","
             << r.insert_time_ms << "," << r.query_time_ms << ","
             << r.total_time_ms << "," << r.checksum << "\n";
    }
    csv1.close();
    
    cout << "\nResults saved to:" << endl;
    cout << "  - experiment1_nc_results.csv" << endl;
}

// ============================================================================
// MAIN
// ============================================================================

int main(int argc, char* argv[]) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    bool do_n_smaller_than_c = false;
    bool do_n_equals_c = false;

    if (argc == 1) {
        do_n_smaller_than_c = true;
    } else {
        for (int i = 1; i < argc; ++i) {
            string arg = argv[i];
            if (arg == "--n_smaller_than_c") {
                do_n_smaller_than_c = true;
            } else if (arg == "--n_equals_c") {
                do_n_equals_c = true;
            } else {
                cerr << "Usage: " << argv[0] << " [options]" << endl;
                cerr << "Options:" << endl;
                cerr << "  --n_smaller_than_c   Run standard benchmarks (N << C)" << endl;
                cerr << "  --n_equals_c         Run experiments where N = C" << endl;
                return 1;
            }
        }
    }

    if (do_n_smaller_than_c) run_n_smaller_than_c();
    if (do_n_equals_c) run_n_equals_c();

    return 0;
}
