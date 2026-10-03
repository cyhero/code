// 贪心分组：p 人分成 b 组，每人有 d 个标签
// 每组逐个挑人，选能带来最多“新标签”的那个
// 让组内标签尽量多样
#include <bits/stdc++.h>
using namespace std;

// id编号 is_used是否已分组 tags各维标签
struct S {
    int id;
    bool is_used = false;
    vector<string> tags;
};

int main() {
    int p, d;
    cin >> p >> d;
    vector<S> s(p);
    for (int i = 0; i < p; i++) {
        cin >> s[i].id;
        s[i].tags.resize(d);
        for (auto& x : s[i].tags) cin >> x;
    }

    // 按编号升序：分数相同时编号小的先选
    sort(s.begin(), s.end(),
         [](const S& a, const S& b) {
             return a.id < b.id;
         });

    int b;
    cin >> b;
    // 每组 q 人，余下 r 人分给最后 r 组
    int q = p / b, r = p % b;

    for (int i = 0; i < b; i++) {
        // 本组容量：前 b-r 组 q 人，其余 q+1
        int cap = i < b - r ? q : q + 1;
        vector<int> ans;
        // st[g]：本组第 g 维已出现的标签
        vector<set<string>> st(d), best_st;

        for (int j = 0; j < cap; j++) {
            int max_val = -1, max_index = -1;

            // 试每个没用过的人
            for (int k = 0; k < p; k++) {
                vector<set<string>> new_st = st;
                int cur = 0; // 能新增几个标签
                if (!s[k].is_used) {
                    for (int g = 0; g < d; g++) {
                        string& t = s[k].tags[g];
                        if (new_st[g].count(t) == 0) {
                            cur++;
                            new_st[g].insert(t);
                        }
                    }
                    // 严格大于：同分保留编号小的
                    if (cur > max_val) {
                        max_index = k;
                        max_val = cur;
                        best_st = new_st;
                    }
                }
            }

            // 选中最优者，更新本组标签
            st = best_st;
            s[max_index].is_used = true;
            ans.push_back(s[max_index].id);
        }

        // 组内编号升序输出
        sort(ans.begin(), ans.end());
        for (int& y : ans) {
            cout << y << " ";
        }
        cout << endl;
        ans.clear();
    }
    return 0;
}
