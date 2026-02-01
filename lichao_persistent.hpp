#ifndef LICHAO_PERSISTENT_HPP
#define LICHAO_PERSISTENT_HPP

#include <vector>
#include <algorithm>
#include <limits>

using namespace std;

typedef long long ll;

namespace LCP {

const ll INF = 4e18;

struct Line {
    ll k, m;
    ll eval(ll x) const { return k * x + m; }
};

struct Node {
    Line line;
    int left = -1;
    int right = -1;
    
    Node(Line l, int le = -1, int ri = -1) : line(l), left(le), right(ri) {}
};

class PersistentLiChaoTree {
    vector<Node> nodes;
    ll min_x, max_x;

public:
    PersistentLiChaoTree(ll min_val = -1e9, ll max_val = 1e9) : min_x(min_val), max_x(max_val) {
        // Reserve some space to avoid reallocations invalidating references if we used pointers,
        // but we use indices so it's fine.
    }

    // Returns the index of the new root.
    // To start a new tree, pass root = -1.
    int add_line(int root, ll k, ll m) {
        return insert(root, {k, m}, min_x, max_x);
    }

    ll query(int root, ll x) {
        return query_node(root, x, min_x, max_x);
    }
    
    int get_node_count() const {
        return nodes.size();
    }

private:
    int new_node(Line l) {
        nodes.emplace_back(l);
        return nodes.size() - 1;
    }

    // Create a copy of an existing node
    int copy_node(int idx) {
        nodes.push_back(nodes[idx]);
        return nodes.size() - 1;
    }

    int insert(int node_idx, Line new_line, ll l, ll r) {
        if (node_idx == -1) {
            return new_node(new_line);
        }

        int new_idx = copy_node(node_idx);
        ll mid = l + (r - l) / 2;

        // Compare at midpoint
        bool midf = new_line.eval(mid) < nodes[new_idx].line.eval(mid);
        bool lef = new_line.eval(l) < nodes[new_idx].line.eval(l);

        if (midf) {
            swap(nodes[new_idx].line, new_line);
        }

        if (l == r) return new_idx;

        if (lef != midf) {
            nodes[new_idx].left = insert(nodes[new_idx].left, new_line, l, mid);
        } else {
            nodes[new_idx].right = insert(nodes[new_idx].right, new_line, mid + 1, r);
        }
        
        return new_idx;
    }

    ll query_node(int node_idx, ll x, ll l, ll r) {
        if (node_idx == -1) return INF;

        ll mid = l + (r - l) / 2;
        ll val = nodes[node_idx].line.eval(x);

        if (l == r) return val;

        if (x <= mid) {
            return min(val, query_node(nodes[node_idx].left, x, l, mid));
        } else {
            return min(val, query_node(nodes[node_idx].right, x, mid + 1, r));
        }
    }
};

}

#endif
