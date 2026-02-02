#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <string>
#include <fstream>

#include "lichao.hpp"
#include "lichao_static.hpp"
#include "lichao_discrete.hpp"
#include "lichao_segment.hpp"
#include "lichao_iterative.hpp"
#include "lichao_persistent.hpp"
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
            sum += lict.query(op.val1);
        }
    }
    return sum;
}

long long run_lichao_iterative(const vector<Operation>& ops) {
    LCI::LiChaoTreeIterative lict(-1e9 - 7, 1e9 + 7);
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

long long run_lichao_static(const vector<Operation>& ops) {
    LCS::LiChaoTreeStatic lict(-1e9 - 7, 1e9 + 7);
    // Estimate size to reserve memory and prevent reallocations during insert
    // A rough upper bound for Li Chao is O(N log C).
    // For N=200k, log C ~ 60, it can be large, but usually much less nodes are created.
    // Let's reserve conservatively based on N.
    lict.reserve(ops.size() * 2); 
    
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

long long run_lichao_discrete(const vector<Operation>& ops) {
    // 1. Collect coords
    vector<long long> coords;
    coords.reserve(ops.size());
    for (const auto& op : ops) {
        if (op.type == QUERY) {
            coords.push_back(op.val1);
        }
    }
    sort(coords.begin(), coords.end());
    coords.erase(unique(coords.begin(), coords.end()), coords.end());

    // 2. Build Tree
    LCD::LiChaoTreeDiscrete lict(coords);
    
    // 3. Process
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

    // Run Li Chao (Discrete)
    start = high_resolution_clock::now();
    long long sum_lcd = run_lichao_discrete(ops);
    end = high_resolution_clock::now();
    double time_lcd = duration_cast<milliseconds>(end - start).count();

    results.push_back({"Li-Chao (Discrete)", n, dist_name, time_lcd, sum_lcd});

    // Run Li Chao (Iterative)
    start = high_resolution_clock::now();
    long long sum_lci = run_lichao_iterative(ops);
    end = high_resolution_clock::now();
    double time_lci = duration_cast<milliseconds>(end - start).count();

    results.push_back({"Li-Chao (Iterative)", n, dist_name, time_lci, sum_lci});

    // Run CHT
    start = high_resolution_clock::now();
    long long sum_cht = run_cht(ops);
    end = high_resolution_clock::now();
    double time_cht = duration_cast<milliseconds>(end - start).count();

    results.push_back({"Dynamic CHT", n, dist_name, time_cht, sum_cht});

    if (sum_lc != sum_cht || sum_lci != sum_cht) {
        cerr << "ERROR: Checksum mismatch! LC: " << sum_lc << " LCI: " << sum_lci << " CHT: " << sum_cht << endl;
    }
}

void verify_persistent_lichao() {
    cout << "Verifying Li-Chao Persistent..." << endl;
    LCP::PersistentLiChaoTree plict(-100, 100);
    
    // Root 0: Empty (implicit infinite)
    int r0 = -1;
    
    // Root 1: Add y = x + 0. At x=10, val=10.
    int r1 = plict.add_line(r0, 1, 0);
    
    // Root 2: Add y = -x + 10. At x=10, val=0. Better than 10.
    // Based on r1.
    int r2 = plict.add_line(r1, -1, 10);
    
    // Root 3: Based on r1 (NOT r2). Add y = -20.
    int r3 = plict.add_line(r1, 0, -20);
    
    // Checks:
    // Query r1 at 10: y=x -> 10.
    // Query r2 at 10: min(10, 0) -> 0.
    // Query r3 at 10: min(10, -20) -> -20.
    
    long long q1 = plict.query(r1, 10);
    long long q2 = plict.query(r2, 10);
    long long q3 = plict.query(r3, 10);
    
    if (q1 != 10) cerr << "Persistent verify failed r1 at 10: expected 10, got " << q1 << endl;
    if (q2 != 0) cerr << "Persistent verify failed r2 at 10: expected 0, got " << q2 << endl;
    if (q3 != -20) cerr << "Persistent verify failed r3 at 10: expected -20, got " << q3 << endl;
    
    if (q1 == 10 && q2 == 0 && q3 == -20) {
        cout << "Li-Chao Persistent verification passed." << endl;
    }
}

void verify_segment_lichao() {
    cout << "Verifying Li-Chao Segment..." << endl;
    vector<long long> coords = {1, 2, 3, 4, 5, 10, 20};
    LCSeg::LiChaoTreeSegment lict(coords);

    // Segment 1: y = 2x + 1 for x in [1, 3] -> (3, 5, 7)
    lict.add_segment(2, 1, 1, 3);
    
    // Segment 2: y = -x + 10 for x in [3, 10] -> (7, 6, 5, 0) at x=3,4,5,10
    lict.add_segment(-1, 10, 3, 10);

    // Expected:
    // x=1: min(3, INF) = 3
    // x=2: min(5, INF) = 5
    // x=3: min(7, 7) = 7
    // x=4: min(INF, 6) = 6
    // x=5: min(INF, 5) = 5
    // x=10: min(INF, 0) = 0
    // x=20: INF

    long long q1 = lict.query(1);
    long long q3 = lict.query(3);
    long long q4 = lict.query(4);
    long long q20 = lict.query(20);

    if (q1 != 3) cerr << "Segment verification failed at x=1, expected 3, got " << q1 << endl;
    if (q3 != 7) cerr << "Segment verification failed at x=3, expected 7, got " << q3 << endl;
    if (q4 != 6) cerr << "Segment verification failed at x=4, expected 6, got " << q4 << endl;
    if (q20 != LCSeg::INF) cerr << "Segment verification failed at x=20, expected INF, got " << q20 << endl;

    if (q1 == 3 && q3 == 7 && q4 == 6 && q20 == LCSeg::INF) {
        cout << "Li-Chao Segment verification passed." << endl;
    }
}

int main() {
    verify_persistent_lichao();
    verify_segment_lichao();
    vector<int> sizes = {10000, 100000, 200000, 1000000, 10000000};
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
