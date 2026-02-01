#ifndef LICHAO_DISCRETE_HPP
#define LICHAO_DISCRETE_HPP

#include <vector>
#include <algorithm>
#include <limits>

using namespace std;

namespace LCD {

typedef long long ll;
const ll INF = 4e18;

struct Line {
    ll k, m;
    ll eval(ll x) const { return k * x + m; }
};

class LiChaoTreeDiscrete {
    vector<Line> tree;
    vector<ll> coords;
    int n;

    // Use 1-based indexing for the tree
    // node covers range of indices [l, r] in coords
    void insert(int node, int l, int r, Line new_line) {
        int mid = l + (r - l) / 2;
        ll x_mid = coords[mid];
        
        // We compare at the actual coordinate value
        bool better_mid = new_line.eval(x_mid) < tree[node].eval(x_mid);

        if (better_mid) {
            swap(tree[node], new_line);
        }
        if (l == r) return;

        // Intersection logic to decide which side to push
        // If the new line (after swap, it's the worse one at mid) is better at left, go left
        ll x_l = coords[l];
        bool better_l = new_line.eval(x_l) < tree[node].eval(x_l);
        
        if (better_l) {
            insert(node * 2, l, mid, new_line);
        } else {
            insert(node * 2 + 1, mid + 1, r, new_line);
        }
    }

    ll query(int node, int l, int r, int idx) {
        if (l == r) return tree[node].eval(coords[idx]);
        
        int mid = l + (r - l) / 2;
        ll val = tree[node].eval(coords[idx]);
        
        if (idx <= mid) {
            return min(val, query(node * 2, l, mid, idx));
        } else {
            return min(val, query(node * 2 + 1, mid + 1, r, idx));
        }
    }

public:
    // Initialize with sorted unique coordinates
    LiChaoTreeDiscrete(const vector<ll>& sorted_coords) : coords(sorted_coords) {
        n = coords.size();
        // Size 4*N is standard for segment trees
        tree.assign(4 * n + 1, {0, INF});
    }

    void add_line(ll k, ll m) {
        if (n == 0) return;
        insert(1, 0, n - 1, {k, m});
    }

    // Query at real coordinate x (must exist in coords)
    ll query(ll x) {
        if (n == 0) return INF;
        // Use lower_bound to find index. 
        // In the benchmark, we guarantee x exists, but let's be safe or assume caller knows.
        // For strict performance in benchmark, we might want to pre-calculate indices, 
        // but finding index is O(log N) which matches tree depth. 
        // We can integrate index finding here.
        auto it = lower_bound(coords.begin(), coords.end(), x);
        int idx = distance(coords.begin(), it);
        return query(1, 0, n - 1, idx);
    }
};

}

#endif
