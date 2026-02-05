#ifndef LICHAO_HPP
#define LICHAO_HPP

#include <bits/stdc++.h>

using namespace std;

typedef long long llint;

const llint INF = 4e18; // Sufficiently large infinity

namespace LC {

struct Line {
    llint k, m;
    llint eval(llint x) const { return k * x + m; }
};

struct Node {
    Line line;
    Node *left = nullptr, *right = nullptr;

    Node(Line l) : line(l) {}
};

class LiChaoTree {
    Node* root = nullptr;
    llint min_x, max_x;

    void insert(Node* &node, Line new_line, llint l, llint r) {
        if (!node) {
            node = new Node(new_line);
            return;
        }
        llint mid = l + (r - l) / 2;
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

    llint query(Node* node, llint x, llint l, llint r) {
        if (!node) return INF;
        llint mid = l + (r - l) / 2;
        llint val = node->line.eval(x);
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
    LiChaoTree(llint min_val = -1e9, llint max_val = 1e9) : min_x(min_val), max_x(max_val) {}
    
    ~LiChaoTree() {
        destroy(root);
    }

    void add_line(llint k, llint m) {
        insert(root, {k, m}, min_x, max_x);
    }

    llint query(llint x) {
        return query(root, x, min_x, max_x);
    }
    
    void clear() {
        destroy(root);
        root = nullptr;
    }
};

}

#endif
