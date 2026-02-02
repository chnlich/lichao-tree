#ifndef RB_LICHAO_HPP
#define RB_LICHAO_HPP

#include <set>
#include <climits>
#include <algorithm>

namespace RB_LC {

typedef long long ll;

struct Line {
    mutable ll k, m, p;
    bool operator<(const Line& o) const { return k > o.k; } // Sort by k descending for min CHT
    bool operator<(ll x) const { return p < x; }
};

/**
 * Red-Black Tree (std::set) based CHT (Convex Hull Trick).
 * This is the standard "optimization" often confused with Li-Chao 
 * in terms of functionality (maintaining lines with arbitrary slopes).
 * It uses a balanced BST (std::set) to store lines and compute the lower envelope.
 */
class RBLiChaoTree {
    struct LineContainer : std::multiset<Line, std::less<>> {
        static const ll inf = LLONG_MAX;
        ll div(ll a, ll b) {
            return a / b - ((a ^ b) < 0 && a % b);
        }
        bool isect(iterator x, iterator y) {
            if (y == end()) return x->p = inf, 0;
            if (x->k == y->k) x->p = x->m <= y->m ? inf : -inf;
            else x->p = div(y->m - x->m, x->k - y->k);
            return x->p >= y->p;
        }
        void add(ll k, ll m) {
            auto z = insert({k, m, 0}), y = z++, x = y;
            while (isect(y, z)) z = erase(z);
            if (x != begin() && isect(--x, y)) isect(x, y = erase(y));
            while ((y = x) != begin() && (--x)->p >= y->p)
                isect(x, erase(y));
        }
        ll query(ll x) {
            if (empty()) return 4e18; // Large value
            auto l = *lower_bound(x);
            return l.k * x + l.m;
        }
    };

    LineContainer lc;

public:
    void add_line(ll k, ll m) {
        lc.add(k, m);
    }

    ll query(ll x) {
        return lc.query(x);
    }

    void clear() {
        lc.clear();
    }
};

}

#endif
