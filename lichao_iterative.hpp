#ifndef LICHAO_ITERATIVE_HPP
#define LICHAO_ITERATIVE_HPP

#include <algorithm>
#include <limits>

using namespace std;

namespace LCI {

typedef long long ll;
const ll INF = 4e18;

struct Line {
    ll k, m;
    ll eval(ll x) const { return k * x + m; }
};

struct Node {
    Line line;
    Node *left = nullptr, *right = nullptr;

    Node(Line l) : line(l) {}
};

class LiChaoTreeIterative {
    Node* root = nullptr;
    ll min_x, max_x;

    void destroy(Node* node) {
        if (!node) return;
        destroy(node->left);
        destroy(node->right);
        delete node;
    }

public:
    LiChaoTreeIterative(ll min_val = -1e9, ll max_val = 1e9) : min_x(min_val), max_x(max_val) {}
    
    ~LiChaoTreeIterative() {
        destroy(root);
    }

    void add_line(ll k, ll m) {
        Line cur = {k, m};
        Node** ptr = &root;
        ll l = min_x, r = max_x;

        while (true) {
            if (!*ptr) {
                *ptr = new Node(cur);
                return;
            }
            Node* node = *ptr;

            ll mid = l + (r - l) / 2;
            bool lef = cur.eval(l) < node->line.eval(l);
            bool midf = cur.eval(mid) < node->line.eval(mid);

            if (midf) {
                swap(node->line, cur);
            }
            if (l == r) return;

            if (lef != midf) {
                ptr = &node->left;
                r = mid;
            } else {
                ptr = &node->right;
                l = mid + 1;
            }
        }
    }

    ll query(ll x) {
        Node* node = root;
        ll l = min_x, r = max_x;
        ll ans = INF;
        
        while (node) {
            ans = min(ans, node->line.eval(x));
            if (l == r) break;
            
            ll mid = l + (r - l) / 2;
            if (x <= mid) {
                node = node->left;
                r = mid;
            } else {
                node = node->right;
                l = mid + 1;
            }
        }
        return ans;
    }
};

}

#endif
