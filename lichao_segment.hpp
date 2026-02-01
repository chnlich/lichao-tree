#ifndef LICHAO_SEGMENT_HPP
#define LICHAO_SEGMENT_HPP

#include <vector>
#include <algorithm>
#include <limits>

using namespace std;

namespace LCSeg {

typedef long long ll;
const ll INF = 4e18;

struct Line {
    ll k, m;
    ll eval(ll x) const { return k * x + m; }
};

// Segment Li Chao Tree
// Supports adding a line segment y = kx + m for x in [x_l, x_r]
class LiChaoTreeSegment {
    vector<Line> tree;
    vector<ll> coords;
    int n;

    // Standard Li Chao Insert on a specific node (covering coords[l...r])
    // This pushes the line "down" the tree relative to the existing line at this node.
    void insert_internal(int node, int l, int r, Line new_line) {
        int mid = l + (r - l) / 2;
        ll x_mid = coords[mid];
        
        bool better_mid = new_line.eval(x_mid) < tree[node].eval(x_mid);

        if (better_mid) {
            swap(tree[node], new_line);
        }
        if (l == r) return;

        ll x_l = coords[l];
        bool better_l = new_line.eval(x_l) < tree[node].eval(x_l);
        
        if (better_l) {
            insert_internal(node * 2, l, mid, new_line);
        } else {
            insert_internal(node * 2 + 1, mid + 1, r, new_line);
        }
    }

    // Segment Tree update to decompose range [ql, qr]
    void update(int node, int l, int r, int ql, int qr, Line new_line) {
        if (l > qr || r < ql) return;
        
        if (l >= ql && r <= qr) {
            insert_internal(node, l, r, new_line);
            return;
        }

        int mid = l + (r - l) / 2;
        update(node * 2, l, mid, ql, qr, new_line);
        update(node * 2 + 1, mid + 1, r, ql, qr, new_line);
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
    LiChaoTreeSegment(const vector<ll>& sorted_coords) : coords(sorted_coords) {
        n = coords.size();
        tree.assign(4 * n + 1, {0, INF});
    }

    // Add segment: y = kx + m for x in range [x_start, x_end]
    // x_start and x_end must be within the range of coords passed to constructor.
    void add_segment(ll k, ll m, ll x_start, ll x_end) {
        if (n == 0) return;
        
        auto it_l = lower_bound(coords.begin(), coords.end(), x_start);
        auto it_r = upper_bound(coords.begin(), coords.end(), x_end);
        
        if (it_l == coords.end()) return;
        
        int idx_l = distance(coords.begin(), it_l);
        int idx_r = distance(coords.begin(), it_r) - 1;

        if (idx_l > idx_r) return;

        update(1, 0, n - 1, idx_l, idx_r, {k, m});
    }

    // Query at specific coordinate x
    ll query(ll x) {
        if (n == 0) return INF;
        auto it = lower_bound(coords.begin(), coords.end(), x);
        if (it == coords.end() || *it != x) return INF; 
        int idx = distance(coords.begin(), it);
        return query(1, 0, n - 1, idx);
    }
};

}

#endif
