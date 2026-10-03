// 拓扑分层：p 个点、e 条依赖边 u→v
// 每轮同时处理所有入度为 0 的点
// 输出要几轮（几波）才能处理完
#include <bits/stdc++.h>
using namespace std;

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int p, e;
    cin >> p >> e;

    // g[u]：u 指向的点；in[v]：v 的入度
    vector<vector<int>> g(p + 1);
    vector<int> in(p + 1);
    while (e--) {
        int u, v;
        cin >> u >> v;
        g[u].push_back(v);
        in[v]++;
    }

    // 第一波：没有依赖的点
    queue<int> q;
    for (int i = 1; i <= p; i++)
        if (!in[i]) q.push(i);

    int waves = 0;
    while (!q.empty()) {
        waves++;
        // 只弹出本波的点（先记下个数）
        for (int n = q.size(); n--; ) {
            int u = q.front(); q.pop();
            // 删掉 u 的出边，入度归 0 进下一波
            for (int v : g[u])
                if (--in[v] == 0) q.push(v);
        }
    }

    // 注：有环时环上的点永远进不了队
    cout << waves;
}
