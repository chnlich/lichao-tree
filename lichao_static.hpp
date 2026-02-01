#ifndef LICHAO_STATIC_HPP
#define LICHAO_STATIC_HPP

#include <vector>
#include <algorithm>

using namespace std;

namespace LCS {

typedef long long ll;
const ll INF = 4e18;

struct Line {
    ll k, m;
    ll eval(ll x) const { return k * x + m; }
};

struct Node {
    Line line;
    int left = -1;
    int right = -1;
};

class LiChaoTreeStatic {
    vector<Node> pool;
    int root = -1;
    ll min_x, max_x;

    int new_node(Line l) {
        pool.push_back({l, -1, -1});
        return pool.size() - 1;
    }

    void insert(int &node_idx, Line new_line, ll l, ll r) {
        if (node_idx == -1) {
            node_idx = new_node(new_line);
            return;
        }
        
        // Reference to the current node (warning: pool might reallocate if we push_back recursively, 
        // but here we only push_back at the start of the function or in children calls.
        // Actually, be careful with references to vector elements if push_back happens.
        // Use indices strictly.
        
        ll mid = l + (r - l) / 2;
        
        // We must access pool[node_idx] freshly every time after any potential push_back
        bool lef = new_line.eval(l) < pool[node_idx].line.eval(l);
        bool midf = new_line.eval(mid) < pool[node_idx].line.eval(mid);

        if (midf) {
            swap(pool[node_idx].line, new_line);
        }
        if (l == r) return;
        
        if (lef != midf) {
            insert(pool[node_idx].left, new_line, l, mid);
        } else {
            insert(pool[node_idx].right, new_line, mid + 1, r);
        }
    }

    ll query(int node_idx, ll x, ll l, ll r) {
        if (node_idx == -1) return INF;
        
        ll mid = l + (r - l) / 2;
        ll val = pool[node_idx].line.eval(x);
        
        if (l == r) return val;
        
        if (x <= mid) {
            return min(val, query(pool[node_idx].left, x, l, mid));
        } else {
            return min(val, query(pool[node_idx].right, x, mid + 1, r));
        }
    }

public:
    LiChaoTreeStatic(ll min_val = -1e9, ll max_val = 1e9) : min_x(min_val), max_x(max_val) {
        pool.reserve(200000 * 2); // Reserve some space to avoid frequent reallocations
    }

    void add_line(ll k, ll m) {
        insert(root, {k, m}, min_x, max_x);
    }

    ll query(ll x) {
        return query(root, x, min_x, max_x);
    }
    
    void clear() {
        pool.clear();
        root = -1;
    }
    
    void reserve(size_t n) {
        pool.reserve(n);
    }
};

}

#endif
