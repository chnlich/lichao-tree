#ifndef ZKW_LICHAO_HPP
#define ZKW_LICHAO_HPP

#include <algorithm>
#include <vector>

namespace ZKW_LC {

typedef long long ll;
const ll INF = 4e18;

struct Line {
    ll k, m;
    ll eval(ll x) const { return k * x + m; }
};

class ZkwLiChaoTree {
    int n;
    int offset;
    std::vector<Line> tree;
    ll min_x;
    ll max_x;
    double step;
    int size_orig;

    std::vector<ll> coords;

    ll get_x(int idx) const {
        if (!coords.empty()) {
            if (idx < 0) return coords[0];
            if (idx >= (int)coords.size()) return coords.back();
            return coords[idx];
        }
        if (size_orig <= 1) return min_x;
        if (idx >= size_orig) return max_x;
        return min_x + (ll)(idx * step);
    }

public:
    void set_coords(const std::vector<ll>& c) {
        coords = c;
    }
    ZkwLiChaoTree(int size, ll min_val = -1e9, ll max_val = 1e9) 
        : min_x(min_val), max_x(max_val), size_orig(size) {
        n = 1;
        while (n < size) n <<= 1;
        offset = n;
        tree.assign(2 * n, {0, INF});
        if (size > 1)
            step = (double)(max_x - min_x) / (size - 1);
        else
            step = 0;
    }

    void add_line(ll k, ll m) {
        Line cur = {k, m};
        int p = 1;
        int l = 0, r = n - 1;
        while (p < 2 * n) {
            int mid = (l + r) >> 1;
            ll mx = get_x(mid);
            
            ll cur_m = cur.eval(mx);
            ll tree_m = tree[p].eval(mx);

            bool midf = cur_m < tree_m;
            if (midf) std::swap(cur, tree[p]);
            
            if (l == r) break;
            
            ll lx = get_x(l);
            ll lx_cur = cur.eval(lx);
            ll lx_tree = tree[p].eval(lx);
            
            if (lx_cur < lx_tree) {
                p = p << 1;
                r = mid;
            } else {
                p = p << 1 | 1;
                l = mid + 1;
            }
        }
    }

    ll query_by_idx(int idx) {
        ll x = get_x(idx);
        ll res = INF;
        for (int p = idx + offset; p > 0; p >>= 1) {
            res = std::min(res, tree[p].eval(x));
        }
        return res;
    }

    ll query(ll x) {
        int idx;
        if (coords.empty()) {
            if (step == 0) idx = 0;
            else idx = std::max(0, std::min(size_orig - 1, (int)((x - min_x) / step + 0.5)));
        } else {
            auto it = std::lower_bound(coords.begin(), coords.end(), x);
            if (it == coords.end()) idx = coords.size() - 1;
            else idx = std::distance(coords.begin(), it);
        }
        
        ll real_x = get_x(idx);
        ll res = INF;
        for (int p = idx + offset; p > 0; p >>= 1) {
            res = std::min(res, tree[p].eval(real_x));
        }
        return res;
    }
};

}

#endif
