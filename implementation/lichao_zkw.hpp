#ifndef LICHAO_ZKW_HPP
#define LICHAO_ZKW_HPP

#include <vector>
#include <algorithm>
#include <cmath>

namespace LC_ZKW {

using std::vector;
using std::swap;
using std::min;

typedef long long llint;
const llint INF = 4e18;

struct Line {
    llint k, m;
    llint eval(llint x) const { return k * x + m; }
};

class LiChaoTree {
    vector<Line> tree;
    int M;
    llint min_x;

public:
    LiChaoTree(llint min_val, llint max_val) {
        min_x = min_val;
        llint range = max_val - min_val + 1;
        M = 1;
        while (M < range) M <<= 1;
        
        // Use a default line that is effectively infinity
        // k=0, m=INF is a safe neutral element for min query
        tree.resize(2 * M, {0, INF});
    }

    void add_line(llint k, llint m) {
        if (m == INF) return; // Optimization: Never push infinite line
        Line new_line = {k, m};
        int node = 1;
        int l = 0;
        int r = M - 1;

        while (true) {
            int mid = l + (r - l) / 2;
            
            // Compare at midpoint
            // Map mid to actual coordinate for evaluation: mid + min_x
            bool midf = new_line.eval(mid + min_x) < tree[node].eval(mid + min_x);
            
            if (midf) {
                swap(tree[node], new_line);
                // After swap, if the old line was empty (sentinel), we can stop
                if (new_line.m == INF) return;
            }
            
            if (l == r) break;
            
            // Check left to decide direction
            bool lef = new_line.eval(l + min_x) < tree[node].eval(l + min_x);
            
            if (lef) {
                node = 2 * node;
                r = mid;
            } else {
                node = 2 * node + 1;
                l = mid + 1;
            }
        }
    }

    llint query(llint x) {
        if (x < min_x || x >= min_x + M) return INF; // Bound check
        
        int idx = (int)(x - min_x);
        int node = 1;
        llint ans = INF;
        
        // Top-down query with early exit on empty nodes
        // Iterate bits from highest (M/2) down to lowest (1) to traverse path
        for (int s = M >> 1; s > 0; s >>= 1) {
            if (tree[node].m == INF) return ans; // Optimization: Subtree empty
            ans = min(ans, tree[node].eval(x));
            
            if (idx & s) {
                node = (node << 1) | 1;
            } else {
                node = (node << 1);
            }
        }
        // Check leaf
        if (tree[node].m != INF) ans = min(ans, tree[node].eval(x));
        
        return ans;
    }
};

}

#endif
