#include <algorithm>
#include <iostream>
#include <map>
#include <tuple>
#include <vector>

using namespace std;

using ll = long long;

const ll INF = 4000000000000000000LL;

// 线段树：区间加，查区间最大值
// 最大值相同时保留最左下标
struct SegTree {
    int n, size;
    vector<ll> mx, lazy;
    vector<int> pos;

    explicit SegTree(const vector<ll>& base) {
        n = (int)base.size();
        size = 1;
        while (size < n) size <<= 1;
        mx.assign(size * 2, -INF);
        pos.assign(size * 2, 0);
        lazy.assign(size * 2, 0);
        for (int i = 0; i < n; i++) {
            mx[size + i] = base[i];
            pos[size + i] = i;
        }
        for (int i = size - 1; i >= 1; i--) pull(i);
    }

    void pull(int i) {
        int left = i * 2, right = i * 2 + 1;
        // 最大值一样时留左边，字典序更小
        if (mx[left] >= mx[right]) {
            mx[i] = mx[left];
            pos[i] = pos[left];
        } else {
            mx[i] = mx[right];
            pos[i] = pos[right];
        }
    }

    void apply(int i, ll val) {
        mx[i] += val;
        lazy[i] += val;
    }

    void push(int i) {
        if (lazy[i] != 0) {
            apply(i * 2, lazy[i]);
            apply(i * 2 + 1, lazy[i]);
            lazy[i] = 0;
        }
    }

    void add(int left, int right, ll val) {
        if (left >= right || val == 0) return;
        add(1, 0, size, left, right, val);
    }

    void add(int i, int lo, int hi, int left, int right, ll val) {
        if (right <= lo || hi <= left) return;
        if (left <= lo && hi <= right) {
            apply(i, val);
            return;
        }
        push(i);
        int mid = (lo + hi) / 2;
        add(i * 2, lo, mid, left, right, val);
        add(i * 2 + 1, mid, hi, left, right, val);
        pull(i);
    }

    // 空区间返回 false
    // 否则给出最大值和最左下标
    bool query(int left, int right, ll& best, int& at) {
        if (left >= right) return false;
        return query(1, 0, size, left, right, best, at);
    }

    bool query(int i, int lo, int hi, int left, int right, ll& best, int& at) {
        if (right <= lo || hi <= left) return false;
        if (left <= lo && hi <= right) {
            best = mx[i];
            at = pos[i];
            return true;
        }
        push(i);
        int mid = (lo + hi) / 2;
        ll bestL, bestR;
        int atL, atR;
        bool hasL = query(i * 2, lo, mid, left, right, bestL, atL);
        bool hasR = query(i * 2 + 1, mid, hi, left, right, bestR, atR);
        if (!hasL) {
            best = bestR;
            at = atR;
            return hasR;
        }
        if (!hasR || bestL >= bestR) {
            best = bestL;
            at = atL;
            return true;
        }
        best = bestR;
        at = atR;
        return true;
    }
};

// 一次停留：净收益和进出时刻
struct Stay {
    ll gain, L, R;
};

// 收益优先，其次起点、终点更小
Stay betterStay(const Stay& a, const Stay& b) {
    if (a.gain != b.gain) return a.gain > b.gain ? a : b;
    if (a.L != b.L) return a.L < b.L ? a : b;
    return a.R <= b.R ? a : b;
}

struct Ans {
    ll gain = 0;
    vector<pair<ll, ll>> segs;
};

// 收益更大则更新，相等取字典序小的
void relax(Ans& ans, ll gain, vector<pair<ll, ll>> segs) {
    if (gain > ans.gain || (gain == ans.gain && segs < ans.segs)) {
        ans.gain = gain;
        ans.segs = std::move(segs);
    }
}

// c = 0：不按长度扣功力
// 正功力异事全收，再尽量拆成两段
Ans solveZero(const vector<tuple<ll, ll, ll>>& events) {
    vector<ll> times;
    vector<tuple<ll, ll, ll>> positive;
    ll total = 0;
    for (const auto& ev : events) {
        ll a = get<0>(ev), b = get<1>(ev), w = get<2>(ev);
        times.push_back(a);
        times.push_back(b);
        if (w > 0) {
            positive.emplace_back(a, b, w);
            total += w;
        }
    }
    Ans ans;
    if (positive.empty()) return ans;
    sort(times.begin(), times.end());
    times.erase(unique(times.begin(), times.end()), times.end());
    ll start0 = times[0];
    ll endAll = 0;
    ll minEnd = INF;
    for (const auto& ev : positive) {
        ll b = get<1>(ev);
        endAll = max(endAll, b);
        minEnd = min(minEnd, b);
    }
    // 只进一次：最早进，最晚的正功力结束出
    ans.gain = total;
    ans.segs.push_back({start0, endAll});

    // 合并正功力区间，用来判断分界点
    vector<pair<ll, ll>> segs;
    for (const auto& ev : positive) {
        ll a = get<0>(ev), b = get<1>(ev);
        if (a < b) segs.push_back({a, b});
    }
    sort(segs.begin(), segs.end());
    vector<pair<ll, ll>> merged;
    for (const auto& seg : segs) {
        ll a = seg.first, b = seg.second;
        if (merged.empty() || a > merged.back().second) merged.push_back({a, b});
        else merged.back().second = max(merged.back().second, b);
    }
    // 找一个合法分界，拆成两段
    int j = 0;
    for (ll cut : times) {
        if (cut >= endAll) break;
        while (j < (int)merged.size() && merged[j].second <= cut) j++;
        // 落在正功力异事内部的时刻
        // 不能当两段的分界
        if (j < (int)merged.size() && merged[j].first <= cut && cut < merged[j].second) continue;
        if (cut < minEnd) continue;
        auto it = lower_bound(times.begin(), times.end(), cut);
        ll second = times[(it - times.begin()) + 1];
        ans.segs = {{start0, cut}, {second, endAll}};
        break;
    }
    return ans;
}

// c > 0：最多停留两次，求最大净收益
Ans solvePositive(const vector<tuple<ll, ll, ll>>& events, ll c) {
    vector<ll> starts, ends;
    map<ll, vector<pair<ll, ll>>> byEnd, byStart;
    for (const auto& ev : events) {
        ll a = get<0>(ev), b = get<1>(ev), w = get<2>(ev);
        starts.push_back(a);
        ends.push_back(b);
        byEnd[b].push_back({a, w});
        byStart[a].push_back({b, w});
    }
    sort(starts.begin(), starts.end());
    starts.erase(unique(starts.begin(), starts.end()), starts.end());
    sort(ends.begin(), ends.end());
    ends.erase(unique(ends.begin(), ends.end()), ends.end());

    // 按结束时刻扫描：
    // 求每个终点对应的最佳单次停留
    vector<ll> baseL(starts.size());
    for (int i = 0; i < (int)starts.size(); i++) baseL[i] = c * starts[i];
    SegTree treeL(baseL);
    vector<Stay> bestAtEnd;
    for (ll end : ends) {
        for (const auto& item : byEnd[end]) {
            ll a = item.first, w = item.second;
            int pos = (int)(lower_bound(starts.begin(), starts.end(), a) - starts.begin());
            // 起点不超过 a 的叶子
            // 都能罩住这起异事
            treeL.add(0, pos + 1, w);
        }
        int right = (int)(upper_bound(starts.begin(), starts.end(), end) - starts.begin());
        ll best;
        int at;
        if (!treeL.query(0, right, best, at)) continue;
        ll gain = best - c * end;
        if (gain > 0) bestAtEnd.push_back({gain, starts[at], end});
    }

    // 按起点反向扫描：
    // 求每个起点对应的最佳单次停留
    vector<ll> baseR(ends.size());
    for (int i = 0; i < (int)ends.size(); i++) baseR[i] = -c * ends[i];
    SegTree treeR(baseR);
    vector<Stay> bestAtStart;
    for (int i = (int)starts.size() - 1; i >= 0; i--) {
        ll start = starts[i];
        for (const auto& item : byStart[start]) {
            ll b = item.first, w = item.second;
            int pos = (int)(lower_bound(ends.begin(), ends.end(), b) - ends.begin());
            // 终点不早于 b 的叶子
            // 都已计入这起异事
            treeR.add(pos, (int)ends.size(), w);
        }
        int left = (int)(lower_bound(ends.begin(), ends.end(), start) - ends.begin());
        ll best;
        int at;
        if (!treeR.query(left, (int)ends.size(), best, at)) continue;
        ll gain = best + c * start;
        if (gain > 0) bestAtStart.push_back({gain, start, ends[at]});
    }

    // 先记录所有最优单次停留
    Ans ans;
    for (auto& iv : bestAtEnd) relax(ans, iv.gain, {{iv.L, iv.R}});
    for (auto& iv : bestAtStart) relax(ans, iv.gain, {{iv.L, iv.R}});

    // 按终点排序，维护前缀最优
    sort(bestAtEnd.begin(), bestAtEnd.end(), [](const Stay& x, const Stay& y) {
        return x.R < y.R;
    });
    vector<Stay> prefix;
    bool has = false;
    Stay cur{};
    for (auto& iv : bestAtEnd) {
        cur = has ? betterStay(cur, iv) : iv;
        has = true;
        prefix.push_back(cur);
    }
    // 拼两段不相交的停留
    for (auto& iv : bestAtStart) {
        // 第一段终点必须严格早于第二段起点
        int lo = 0, hi = (int)prefix.size();
        while (lo < hi) {
            int mid = (lo + hi) / 2;
            if (bestAtEnd[mid].R < iv.L) lo = mid + 1;
            else hi = mid;
        }
        if (lo == 0) continue;
        Stay left = prefix[lo - 1];
        relax(ans, left.gain + iv.gain, {{left.L, left.R}, {iv.L, iv.R}});
    }
    return ans;
}

Ans solve(const vector<tuple<ll, ll, ll>>& events, ll c) {
    if (events.empty()) return Ans{};
    // 单位消耗为 0 时走字典序分界
    if (c == 0) return solveZero(events);
    return solvePositive(events, c);
}

int main() {
    int p;
    ll c;
    // 首行：异事起数、单位消耗
    if (!(cin >> p >> c)) return 0;
    vector<tuple<ll, ll, ll>> events;
    events.reserve(p);
    for (int i = 0; i < p; i++) {
        ll a, b, w;
        cin >> a >> b >> w;
        events.emplace_back(a, b, w);
    }
    Ans ans = solve(events, c);
    cout << ans.gain << "\n";
    if (ans.segs.empty()) {
        cout << "NA\n";
    } else {
        for (const auto& seg : ans.segs) cout << seg.first << "," << seg.second << "\n";
    }
    return 0;
}
