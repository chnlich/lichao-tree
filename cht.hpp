#ifndef CHT_HPP
#define CHT_HPP

#include <set>
#include <climits>
#include <algorithm>

using namespace std;

typedef long long ll;

namespace CHT {

struct Line {
	mutable ll k, m, p;
	bool operator<(const Line& o) const { return k < o.k; }
	bool operator<(ll x) const { return p < x; }
};

struct LineContainer : multiset<Line, less<>> {
	// (for doubles, use inf = 1/.0, div(a,b) = a/b)
	static const ll inf = LLONG_MAX;
	ll div(ll a, ll b) { // floored division
		return a / b - ((a ^ b) < 0 && a % b); }
	bool isect(iterator x, iterator y) {
		if (y == end()) return x->p = inf, 0;
		if (x->k == y->k) x->p = x->m > y->m ? inf : -inf;
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
		if (empty()) return 0;
		auto l = *lower_bound(x);
		return l.k * x + l.m;
	}
    void clear_all() {
        clear();
    }
};

// Wrapper for Min Hull
class DynamicCHT {
    LineContainer lc;
public:
    void add_line(ll k, ll m) {
        // To query min(kx + m), we query max(-kx - m) and negate result
        lc.add(-k, -m);
    }

    ll query(ll x) {
        if (lc.empty()) return 4e18; // Or some large error value
        return -lc.query(x);
    }
    
    void clear() {
        lc.clear_all();
    }
};

}

#endif
