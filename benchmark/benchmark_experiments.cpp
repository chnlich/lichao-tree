#include <bits/stdc++.h>
#include "../implementation/lichao.hpp"
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
    
    // Generate operations with N lines and C coordinate range
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
};

NCResult run_nc_lichao(const vector<pair<llint,llint>>& inserts, const vector<llint>& queries, llint c) {
    NCResult res;
    llint half_c = c / 2;
    LC::LiChaoTree lict(-half_c - 1, half_c + 1);
    
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
    
    // Test cases: N = C = 100,000 and N = C = 1,000,000
    vector<pair<int, llint>> test_cases = {{100000, 100000}, {1000000, 1000000}};
    
    for (const auto& tc : test_cases) {
        int n = tc.first;
        llint c = tc.second;
        
        cout << "Testing N=" << n << ", C=" << c << "..." << endl;
        
        NCGenerator gen(42, c);
        auto [inserts, queries] = gen.generate_nc(n, c);
        
        // Run LICT
        auto res_lichao = run_nc_lichao(inserts, queries, c);
        res_lichao.n = n;
        res_lichao.c = c;
        results.push_back(res_lichao);
        
        // Run CHT
        auto res_cht = run_nc_cht(inserts, queries, c);
        res_cht.n = n;
        res_cht.c = c;
        results.push_back(res_cht);
        
        // Verify checksums match
        if (res_lichao.checksum != res_cht.checksum) {
            cerr << "WARNING: Checksum mismatch! LICT: " << res_lichao.checksum 
                 << " CHT: " << res_cht.checksum << endl;
        }
    }
    
    return results;
}

// ============================================================================
// EXPERIMENT 2: FLOATING-POINT COMPARISON
// ============================================================================

// Double-precision Li Chao Tree
namespace LC_Double {

struct Line {
    double k, m;
    double eval(double x) const { return k * x + m; }
};

struct Node {
    Line line;
    Node *left = nullptr, *right = nullptr;
    Node(Line l) : line(l) {}
};

class LiChaoTree {
    Node* root = nullptr;
    double min_x, max_x;
    const double INF = 1e300;

    void insert(Node* &node, Line new_line, double l, double r) {
        if (!node) {
            node = new Node(new_line);
            return;
        }
        double mid = l + (r - l) / 2.0;
        bool lef = new_line.eval(l) < node->line.eval(l);
        bool midf = new_line.eval(mid) < node->line.eval(mid);

        if (midf) {
            swap(node->line, new_line);
        }
        if (l == r || fabs(r - l) < 1e-9) return;
        if (lef != midf) {
            insert(node->left, new_line, l, mid);
        } else {
            insert(node->right, new_line, mid, r);
        }
    }

    double query(Node* node, double x, double l, double r) {
        if (!node) return INF;
        double mid = l + (r - l) / 2.0;
        double val = node->line.eval(x);
        if (l == r || fabs(r - l) < 1e-9) return val;
        if (x <= mid) {
            return min(val, query(node->left, x, l, mid));
        } else {
            return min(val, query(node->right, x, mid, r));
        }
    }

    void destroy(Node* node) {
        if (!node) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

public:
    LiChaoTree(double min_val = -1e6, double max_val = 1e6) : min_x(min_val), max_x(max_val) {}
    
    ~LiChaoTree() {
        destroy(root);
    }

    void add_line(double k, double m) {
        insert(root, {k, m}, min_x, max_x);
    }

    double query(double x) {
        double res = query(root, x, min_x, max_x);
        return (res >= INF/2) ? NAN : res;
    }
    
    void clear() {
        destroy(root);
        root = nullptr;
    }
};

}

// Double-precision CHT
namespace CHT_Double {

struct Line {
    mutable double k, m, p;
    bool operator<(const Line& o) const { return k < o.k; }
    bool operator<(double x) const { return p < x; }
};

struct LineContainer : multiset<Line, less<>> {
    static const double inf;
    
    double div(double a, double b) {
        return a / b;
    }
    
    bool isect(iterator x, iterator y) {
        if (y == end()) return x->p = inf, 0;
        if (fabs(x->k - y->k) < 1e-12) {
            x->p = x->m > y->m ? inf : -inf;
        } else {
            x->p = div(y->m - x->m, x->k - y->k);
        }
        return x->p >= y->p;
    }
    
    void add(double k, double m) {
        auto z = insert({k, m, 0}), y = z++, x = y;
        while (isect(y, z)) z = erase(z);
        if (x != begin() && isect(--x, y)) isect(x, y = erase(y));
        while ((y = x) != begin() && (--x)->p >= y->p)
            isect(x, erase(y));
    }
    
    double query(double x) {
        if (empty()) return 0;
        auto l = *lower_bound(x);
        return l.k * x + l.m;
    }
    
    void clear_all() {
        clear();
    }
};

const double LineContainer::inf = 1e300;

class DynamicCHT {
    LineContainer lc;
public:
    void add_line(double k, double m) {
        lc.add(-k, -m);
    }

    double query(double x) {
        if (lc.empty()) return NAN;
        return -lc.query(x);
    }
    
    void clear() {
        lc.clear_all();
    }
};

}

struct FPResult {
    int n;
    string type; // "int" or "double"
    string algorithm;
    double insert_time_ms;
    double query_time_ms;
    double total_time_ms;
    double checksum;
    int precision_issues; // Count of NaN or inf results
};

// Generator for floating-point experiment
class FPGenerator {
    mt19937_64 rng;
public:
    FPGenerator(llint seed) : rng(seed) {}
    
    // Generate integer operations
    pair<vector<pair<llint,llint>>, vector<llint>> generate_int(int n) {
        vector<pair<llint,llint>> inserts;
        vector<llint> queries;
        
        uniform_int_distribution<llint> dist_k(-1000000, 1000000);
        uniform_int_distribution<llint> dist_b(-1000000, 1000000);
        uniform_int_distribution<llint> dist_x(-1000000, 1000000);
        
        for (int i = 0; i < n / 2; ++i) {
            inserts.push_back({dist_k(rng), dist_b(rng)});
        }
        for (int i = 0; i < n / 2; ++i) {
            queries.push_back(dist_x(rng));
        }
        
        return {inserts, queries};
    }
    
    // Generate double operations
    pair<vector<pair<double,double>>, vector<double>> generate_double(int n) {
        vector<pair<double,double>> inserts;
        vector<double> queries;
        
        uniform_real_distribution<double> dist_k(-1e6, 1e6);
        uniform_real_distribution<double> dist_b(-1e6, 1e6);
        uniform_real_distribution<double> dist_x(-1e6, 1e6);
        
        for (int i = 0; i < n / 2; ++i) {
            inserts.push_back({dist_k(rng), dist_b(rng)});
        }
        for (int i = 0; i < n / 2; ++i) {
            queries.push_back(dist_x(rng));
        }
        
        return {inserts, queries};
    }
};

FPResult run_fp_lichao_int(const vector<pair<llint,llint>>& inserts, const vector<llint>& queries) {
    FPResult res;
    LC::LiChaoTree lict(-1000000, 1000000);
    
    auto start = high_resolution_clock::now();
    for (const auto& ins : inserts) {
        lict.add_line(ins.first, ins.second);
    }
    auto mid = high_resolution_clock::now();
    
    long long sum = 0;
    int issues = 0;
    for (llint x : queries) {
        llint val = lict.query(x);
        if (val >= 4e18) issues++;
        else sum += val;
    }
    auto end = high_resolution_clock::now();
    
    res.insert_time_ms = duration_cast<microseconds>(mid - start).count() / 1000.0;
    res.query_time_ms = duration_cast<microseconds>(end - mid).count() / 1000.0;
    res.total_time_ms = duration_cast<microseconds>(end - start).count() / 1000.0;
    res.checksum = (double)sum;
    res.precision_issues = issues;
    res.algorithm = "LICT";
    res.type = "int";
    
    return res;
}

FPResult run_fp_cht_int(const vector<pair<llint,llint>>& inserts, const vector<llint>& queries) {
    FPResult res;
    CHT::DynamicCHT cht;
    
    auto start = high_resolution_clock::now();
    for (const auto& ins : inserts) {
        cht.add_line(ins.first, ins.second);
    }
    auto mid = high_resolution_clock::now();
    
    long long sum = 0;
    int issues = 0;
    for (llint x : queries) {
        llint val = cht.query(x);
        if (val >= 4e18) issues++;
        else sum += val;
    }
    auto end = high_resolution_clock::now();
    
    res.insert_time_ms = duration_cast<microseconds>(mid - start).count() / 1000.0;
    res.query_time_ms = duration_cast<microseconds>(end - mid).count() / 1000.0;
    res.total_time_ms = duration_cast<microseconds>(end - start).count() / 1000.0;
    res.checksum = (double)sum;
    res.precision_issues = issues;
    res.algorithm = "Dynamic CHT";
    res.type = "int";
    
    return res;
}

FPResult run_fp_lichao_double(const vector<pair<double,double>>& inserts, const vector<double>& queries) {
    FPResult res;
    LC_Double::LiChaoTree lict(-1e6, 1e6);
    
    auto start = high_resolution_clock::now();
    for (const auto& ins : inserts) {
        lict.add_line(ins.first, ins.second);
    }
    auto mid = high_resolution_clock::now();
    
    double sum = 0;
    int issues = 0;
    for (double x : queries) {
        double val = lict.query(x);
        if (std::isnan(val) || std::isinf(val)) issues++;
        else sum += val;
    }
    auto end = high_resolution_clock::now();
    
    res.insert_time_ms = duration_cast<microseconds>(mid - start).count() / 1000.0;
    res.query_time_ms = duration_cast<microseconds>(end - mid).count() / 1000.0;
    res.total_time_ms = duration_cast<microseconds>(end - start).count() / 1000.0;
    res.checksum = sum;
    res.precision_issues = issues;
    res.algorithm = "LICT";
    res.type = "double";
    
    return res;
}

FPResult run_fp_cht_double(const vector<pair<double,double>>& inserts, const vector<double>& queries) {
    FPResult res;
    CHT_Double::DynamicCHT cht;
    
    auto start = high_resolution_clock::now();
    for (const auto& ins : inserts) {
        cht.add_line(ins.first, ins.second);
    }
    auto mid = high_resolution_clock::now();
    
    double sum = 0;
    int issues = 0;
    for (double x : queries) {
        double val = cht.query(x);
        if (std::isnan(val) || std::isinf(val)) issues++;
        else sum += val;
    }
    auto end = high_resolution_clock::now();
    
    res.insert_time_ms = duration_cast<microseconds>(mid - start).count() / 1000.0;
    res.query_time_ms = duration_cast<microseconds>(end - mid).count() / 1000.0;
    res.total_time_ms = duration_cast<microseconds>(end - start).count() / 1000.0;
    res.checksum = sum;
    res.precision_issues = issues;
    res.algorithm = "Dynamic CHT";
    res.type = "double";
    
    return res;
}

vector<FPResult> experiment2_floating_point() {
    cout << "\n=== EXPERIMENT 2: Floating-Point Comparison ===\n" << endl;
    
    vector<FPResult> results;
    vector<int> sizes = {100000, 1000000};
    
    for (int n : sizes) {
        cout << "Testing N=" << n << "..." << endl;
        
        FPGenerator gen(42);
        
        // Integer baseline
        auto [inserts_int, queries_int] = gen.generate_int(n);
        auto res_lichao_int = run_fp_lichao_int(inserts_int, queries_int);
        res_lichao_int.n = n;
        results.push_back(res_lichao_int);
        
        auto res_cht_int = run_fp_cht_int(inserts_int, queries_int);
        res_cht_int.n = n;
        results.push_back(res_cht_int);
        
        // Double precision
        auto [inserts_double, queries_double] = gen.generate_double(n);
        auto res_lichao_double = run_fp_lichao_double(inserts_double, queries_double);
        res_lichao_double.n = n;
        results.push_back(res_lichao_double);
        
        auto res_cht_double = run_fp_cht_double(inserts_double, queries_double);
        res_cht_double.n = n;
        results.push_back(res_cht_double);
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
    
    // Run Experiment 2: Floating-Point Comparison
    auto fp_results = experiment2_floating_point();
    
    // Output results in markdown format
    cout << "\n\n=================================================================" << endl;
    cout << "RESULTS SUMMARY" << endl;
    cout << "=================================================================" << endl;
    
    // Experiment 1 Results Table
    cout << "\n## Experiment 1: N = C Regime\n" << endl;
    cout << "| N = C | Algorithm | Insert Time (ms) | Query Time (ms) | Total Time (ms) |" << endl;
    cout << "|-------|-----------|------------------|-----------------|-----------------|" << endl;
    for (const auto& r : nc_results) {
        cout << "| " << r.n << " | " << r.algorithm << " | " 
             << fixed << setprecision(2) << r.insert_time_ms << " | "
             << r.query_time_ms << " | " << r.total_time_ms << " |" << endl;
    }
    
    // Experiment 2 Results Table
    cout << "\n## Experiment 2: Floating-Point Comparison\n" << endl;
    cout << "| N | Type | Algorithm | Insert Time (ms) | Query Time (ms) | Total (ms) | Issues |" << endl;
    cout << "|---|------|-----------|------------------|-----------------|------------|--------|" << endl;
    for (const auto& r : fp_results) {
        cout << "| " << r.n << " | " << r.type << " | " << r.algorithm << " | "
             << fixed << setprecision(2) << r.insert_time_ms << " | "
             << r.query_time_ms << " | " << r.total_time_ms << " | "
             << r.precision_issues << " |" << endl;
    }
    
    // Save to CSV
    ofstream csv1("experiment1_nc_results.csv");
    csv1 << "N,C,Algorithm,Insert_Time_ms,Query_Time_ms,Total_Time_ms,Checksum\n";
    for (const auto& r : nc_results) {
        csv1 << r.n << "," << r.c << "," << r.algorithm << ","
             << r.insert_time_ms << "," << r.query_time_ms << ","
             << r.total_time_ms << "," << r.checksum << "\n";
    }
    csv1.close();
    
    ofstream csv2("experiment2_fp_results.csv");
    csv2 << "N,Type,Algorithm,Insert_Time_ms,Query_Time_ms,Total_Time_ms,Precision_Issues\n";
    for (const auto& r : fp_results) {
        csv2 << r.n << "," << r.type << "," << r.algorithm << ","
             << r.insert_time_ms << "," << r.query_time_ms << ","
             << r.total_time_ms << "," << r.precision_issues << "\n";
    }
    csv2.close();
    
    cout << "\nResults saved to:" << endl;
    cout << "  - experiment1_nc_results.csv" << endl;
    cout << "  - experiment2_fp_results.csv" << endl;
    
    return 0;
}
