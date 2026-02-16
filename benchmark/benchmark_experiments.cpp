#include "../implementation/lichao.hpp"
#include "../implementation/lichao_zkw.hpp"
#include "../implementation/cht.hpp"

using namespace std;
using namespace std::chrono;

// ============================================================================
// EXPERIMENT 1: N = C REGIME
// ============================================================================
// In this experiment, we set N (number of operations) equal to C (coordinate range)
// This tests the regime where LICT's O(log C) and CHT's O(log n) directly compare

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

// Generator for N=C experiment
class NCGenerator {
    mt19937_64 rng;
    llint range;
public:
    NCGenerator(llint seed, llint coord_range) : rng(seed), range(coord_range) {}
    
    llint random(llint l, llint r) {
        uniform_int_distribution<llint> dist(l, r);
        return dist(rng);
    }
    
    // Generate random operations with N lines and C coordinate range
    // Returns pair of (insert_ops, query_ops) for separate timing
    pair<vector<pair<llint,llint>>, vector<llint>> generate_nc(int n, llint c) {
        vector<pair<llint,llint>> inserts;
        vector<llint> queries;
        
        llint half_c = c / 2;
        
        // Generate n/2 insertions
        for (int i = 0; i < n / 2; ++i) {
            llint k = random(-half_c, half_c);
            llint b = random(-half_c, half_c);
            inserts.push_back({k, b});
        }
        
        // Generate n/2 queries
        for (int i = 0; i < n / 2; ++i) {
            llint x = random(-half_c, half_c);
            queries.push_back(x);
        }
        
        return {inserts, queries};
    }
    
    // Generate all-on-hull operations: y = i*x - i^2, all tangent to parabola
    pair<vector<pair<llint,llint>>, vector<llint>> generate_nc_all_on_hull(int n, llint c) {
        vector<pair<llint,llint>> inserts;
        vector<llint> queries;
        
        // Generate n/2 insertions: all lines on lower envelope (min hull)
        // Lines: y = -i*x + i^2, tangent to parabola y = -x^2/4
        for (int i = 0; i < n / 2; ++i) {
            llint k = -(i + 1);
            llint m = (llint)(i + 1) * (i + 1);
            inserts.push_back({k, m});
        }
        
        // Shuffle to avoid monotonic slope insertion order
        shuffle(inserts.begin(), inserts.end(), rng);
        
        // Generate n/2 queries in [0, n] to cover intersection points (at x = 3, 5, ..., n-1)
        for (int i = 0; i < n / 2; ++i) {
            llint x = random(0, (llint)n);
            queries.push_back(x);
        }
        
        return {inserts, queries};
    }
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

NCResult run_nc_cht(const vector<pair<llint,llint>>& inserts, const vector<llint>& queries, llint c) {
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

vector<NCResult> experiment1_nc_regime() {
    cout << "\n=== EXPERIMENT 1: N = C Regime ===\n" << endl;
    
    vector<NCResult> results;
    
    vector<pair<int, llint>> test_cases = {
        {100000, 100000}, 
        {1000000, 1000000}, 
        {10000000, 10000000},
        {8388608, 8388608} // 2^23, testing perfect power-of-two alignment
    };
    vector<string> distributions = {"Random", "All on Hull"};
    
    for (const auto& tc : test_cases) {
        int n = tc.first;
        llint c = tc.second;
        
        for (const auto& dist : distributions) {
            cout << "Testing N=" << n << ", C=" << c << ", Dist=" << dist << "..." << endl;
            
            NCGenerator gen(42, c);
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
    
    return results;
}

// ============================================================================
// MAIN
// ============================================================================

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
    cout << "=================================================================" << endl;
    cout << "NEW BENCHMARK EXPERIMENTS FOR LICT vs CHT COMPARISON" << endl;
    cout << "=================================================================" << endl;
    
    // Run Experiment 1: N = C Regime
    auto nc_results = experiment1_nc_regime();
    
    // Output results in markdown format
    cout << "\n\n=================================================================" << endl;
    cout << "RESULTS SUMMARY" << endl;
    cout << "=================================================================" << endl;
    
    cout << "\n## Experiment 1: N = C Regime\n" << endl;
    cout << "| N = C | Distribution | Algorithm | Insert Time (ms) | Query Time (ms) | Total Time (ms) |" << endl;
    cout << "|-------|-------------|-----------|------------------|-----------------|-----------------|" << endl;
    for (const auto& r : nc_results) {
        cout << "| " << r.n << " | " << r.distribution << " | " << r.algorithm << " | " 
             << fixed << setprecision(2) << r.insert_time_ms << " | "
             << r.query_time_ms << " | " << r.total_time_ms << " |" << endl;
    }
    
    // Save to CSV
    ofstream csv1("experiment1_nc_results.csv");
    csv1 << "N,C,Distribution,Algorithm,Insert_Time_ms,Query_Time_ms,Total_Time_ms,Checksum\n";
    for (const auto& r : nc_results) {
        csv1 << r.n << "," << r.c << "," << r.distribution << "," << r.algorithm << ","
             << r.insert_time_ms << "," << r.query_time_ms << ","
             << r.total_time_ms << "," << r.checksum << "\n";
    }
    csv1.close();
    
    cout << "\nResults saved to:" << endl;
    cout << "  - experiment1_nc_results.csv" << endl;
    
    return 0;
}

