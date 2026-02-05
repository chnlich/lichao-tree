#ifndef CHT_HPP
#define CHT_HPP

#include <bits/stdc++.h>

using namespace std;

typedef long long llint;

namespace CHT {

struct Line {
	mutable llint k, m, p;
	bool operator<(const Line& o) const { return k < o.k; }
	bool operator<(llint x) const { return p < x; }
};

struct LineContainer : multiset<Line, less<>> {
	// (for doubles, use inf = 1/.0, div(a,b) = a/b)
	static const llint inf = LLONG_MAX;
	llint div(llint a, llint b) { // floored division
		return a / b - ((a ^ b) < 0 && a % b); }
	bool isect(iterator x, iterator y) {
		if (y == end()) return x->p = inf, 0;
		if (x->k == y->k) x->p = x->m > y->m ? inf : -inf;
		else x->p = div(y->m - x->m, x->k - y->k);
		return x->p >= y->p;
	}
	void add(llint k, llint m) {
		auto z = insert({k, m, 0}), y = z++, x = y;
		while (isect(y, z)) z = erase(z);
		if (x != begin() && isect(--x, y)) isect(x, y = erase(y));
		while ((y = x) != begin() && (--x)->p >= y->p)
			isect(x, erase(y));
	}
	llint query(llint x) {
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
    void add_line(llint k, llint m) {
        // To query min(kx + m), we query max(-kx - m) and negate result
        lc.add(-k, -m);
    }

    llint query(llint x) {
        if (lc.empty()) return 4e18; // Or some large error value
        return -lc.query(x);
    }
    
    void clear() {
        lc.clear_all();
    }
};

}

#endif
