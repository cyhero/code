// 样品检测调度：每类样品只能用同类设备
// 设备空闲就从排队的样品里挑最优先的来做
#include <bits/stdc++.h>
using namespace std;
typedef long long ll;

// t到达时刻 d耗时 c类型 p优先级 id编号
struct S { ll t, d; int c, p, id; };

// 排队规则（返回 true 表示 a 排在 b 后面）
struct Cmp {
    bool operator()(const S& a, const S& b) const {
        if (a.p != b.p) return a.p < b.p; // 优先级高先
        if (a.t != b.t) return a.t > b.t; // 先到先
        return a.id > b.id;               // 编号小先
    }
};

int main() {
    int m, n;
    cin >> m >> n;

    // cnt[c]：第 c 类设备有几台
    vector<int> cnt(4, 0);
    for (int i = 0; i < m; i++) {
        int a; cin >> a; cnt[a]++;
    }

    vector<S> s(n);
    for (int i = 0; i < n; i++) {
        cin >> s[i].t >> s[i].c >> s[i].d >> s[i].p;
        s[i].id = i;
    }

    vector<ll> st(n), ed(n); // 开始、结束时刻

    // 三类互不影响，分开模拟
    for (int c = 1; c <= 3; c++) {
        vector<S> v;
        for (auto& x : s)
            if (x.c == c) v.push_back(x);
        if (v.empty() || cnt[c] == 0) continue;

        // 按到达时间排序
        sort(v.begin(), v.end(),
             [](const S& a, const S& b) {
                 if (a.t != b.t) return a.t < b.t;
                 return a.id < b.id;
             });

        // 小根堆：各设备何时空闲
        priority_queue<ll, vector<ll>,
                       greater<ll>> dev;
        for (int k = 0; k < cnt[c]; k++)
            dev.push(0);

        // 已到达、正在排队的样品
        priority_queue<S, vector<S>, Cmp> wait;

        size_t i = 0;
        while (i < v.size() || !wait.empty()) {
            // 取最早空出来的设备
            ll T = dev.top(); dev.pop();

            // 没人排队：设备等到下个样品到达
            if (wait.empty() && v[i].t > T)
                T = v[i].t;

            // 此刻前到达的都进队
            while (i < v.size() && v[i].t <= T)
                wait.push(v[i++]);

            // 挑最优先的开工
            S x = wait.top(); wait.pop();
            st[x.id] = T;
            ed[x.id] = T + x.d;
            dev.push(ed[x.id]);
        }
    }

    for (int i = 0; i < n; i++)
        printf("%lld %lld\n", st[i], ed[i]);
}
