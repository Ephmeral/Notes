## [剑指 Offer 13. 机器人的运动范围](https://leetcode.cn/problems/ji-qi-ren-de-yun-dong-fan-wei-lcof/)

地上有一个m行n列的方格，从坐标 `[0,0]` 到坐标 `[m-1,n-1]` 。一个机器人从坐标 `[0, 0]` 的格子开始移动，它每次可以向左、右、上、下移动一格（不能移动到方格外），也不能进入行坐标和列坐标的数位之和大于k的格子。例如，当k为18时，机器人能够进入方格 [35, 37] ，因为3+5+3+7=18。但它不能进入方格 [35, 38]，因为3+5+3+8=19。请问该机器人能够到达多少个格子？

**示例 1：**

**输入：**m = 2, n = 3, k = 1
**输出：**3

**示例 2：**

**输入：**m = 3, n = 1, k = 0
**输出：**1

**提示：**

- `1 <= n,m <= 100`
- `0 <= k <= 20`

### 解题思路

DFS，递归枚举所有相邻的点，如何计算总和，需要用 vis 数组标记是否访问过

```cpp
class Solution {
    const int dx[4] = {0, 0, -1, 1};
    const int dy[4] = {1, 1, -1, 0};
public:
    int movingCount(int m, int n, int k) {
        vector<vector<int>> vis(m, vector<int>(n, 0));
        return dfs(0, 0, m, n, k, vis);
    }

    int dfs(int x, int y, int m, int n, int k, vector<vector<int>> &vis) {
        if (!(x >= 0 && x < m && y >= 0 && y < n)) {
            return 0;
        }
        if (check(x, y) > k) {
            return 0;
        }
        int res = 1;
        vis[x][y] = 1;
        for (int i = 0; i < 4; ++i) {
            int mx = x + dx[i];
            int my = y + dy[i];
            if ((mx >= 0 && mx < m && my >= 0 && my < n) && check(mx, my) <= k && vis[mx][my] == 0) {
                res += dfs(mx, my, m, n, k, vis);
            }
        }
        return res;
    }

    int check(int x, int y) {
        int res = 0;
        while (x) {
            res += x % 10;
            x /= 10;
        }
        while (y) {
            res += y % 10;
            y /= 10;
        }
        return res;
    }
};
```