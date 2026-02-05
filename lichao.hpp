#ifndef LICHAO_HPP
#define LICHAO_HPP

#include <algorithm>
#include <limits>

using namespace std;

typedef long long ll;

const ll INF = 4e18; // Sufficiently large infinity

namespace LC {

struct Line {
    ll k, m;
    ll eval(ll x) const { return k * x + m; }
};

struct Node {
    Line line;
    Node *left = nullptr, *right = nullptr;

    Node(Line l) : line(l) {}
};

class LiChaoTree {
    Node* root = nullptr;
    ll min_x, max_x;

    void insert(Node* &node, Line new_line, ll l, ll r) {
        if (!node) {
            node = new Node(new_line);
            return;
        }
        ll mid = l + (r - l) / 2;
        bool lef = new_line.eval(l) < node->line.eval(l);
        bool midf = new_line.eval(mid) < node->line.eval(mid);

        if (midf) {
            swap(node->line, new_line);
        }
        if (l == r) return;
        if (lef != midf) {
            insert(node->left, new_line, l, mid);
        } else {
            insert(node->right, new_line, mid + 1, r);
        }
    }

    ll query(Node* node, ll x, ll l, ll r) {
        if (!node) return INF;
        ll mid = l + (r - l) / 2;
        ll val = node->line.eval(x);
        if (l == r) return val;
        if (x <= mid) {
            return min(val, query(node->left, x, l, mid));
        } else {
            return min(val, query(node->right, x, mid + 1, r));
        }
    }

    void destroy(Node* node) {
        if (!node) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

public:
    LiChaoTree(ll min_val = -1e9, ll max_val = 1e9) : min_x(min_val), max_x(max_val) {}
    
    ~LiChaoTree() {
        destroy(root);
    }

    void add_line(ll k, ll m) {
        insert(root, {k, m}, min_x, max_x);
    }

    ll query(ll x) {
        return query(root, x, min_x, max_x);
    }
    
    void clear() {
        destroy(root);
        root = nullptr;
    }
};

}

#endif
