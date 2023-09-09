## [剑指 Offer 47. 礼物的最大价值](https://leetcode.cn/problems/li-wu-de-zui-da-jie-zhi-lcof/)

在一个 m\*n 的棋盘的每一格都放有一个礼物，每个礼物都有一定的价值（价值大于 0）。你可以从棋盘的左上角开始拿格子里的礼物，并每次向右或者向下移动一格、直到到达棋盘的右下角。给定一个棋盘及其上面的礼物的价值，请计算你最多能拿到多少价值的礼物？

**示例 1:**

**输入:** 
`[   [1,3,1],   [1,5,1],   [4,2,1] ]`
**输出:** `12`
**解释:** 路径 1→3→5→2→1 可以拿到最多价值的礼物

提示：

- `0 < grid.length <= 200`
- `0 < grid[0].length <= 200`

## 解题思路

简单的动态规划，转移方程就是从左边或者上面的格子转移过来，注意边界情况

```cpp
class Solution {
public:
    int maxValue(vector<vector<int>>& grid) {
        int m = grid.size(), n = grid[0].size();
        vector<vector<int>> dp(m + 1, vector<int>(n + 1, 0));
        dp[1][1] = grid[0][0];
        for (int i = 1; i <= m; ++i) {
            for (int j = 1; j <= n; ++j) {
                if (i == 1 && j == 1) continue;
                dp[i][j] = max(dp[i - 1][j], dp[i][j - 1]);
                dp[i][j] += grid[i - 1][j - 1];
            }
        }
        return *max_element(dp[m].begin(), dp[m].end());
    }
};
```