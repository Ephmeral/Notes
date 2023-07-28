## [剑指 Offer 12. 矩阵中的路径](https://leetcode.cn/problems/ju-zhen-zhong-de-lu-jing-lcof/)

给定一个 `m x n` 二维字符网格 `board` 和一个字符串单词 `word` 。如果 `word` 存在于网格中，返回 `true` ；否则，返回 `false` 。

单词必须按照字母顺序，通过相邻的单元格内的字母构成，其中“相邻”单元格是那些水平相邻或垂直相邻的单元格。同一个单元格内的字母不允许被重复使用。

例如，在下面的 3×4 的矩阵中包含单词 "ABCCED"（单词中的字母已标出）。

![](https://assets.leetcode.com/uploads/2020/11/04/word2.jpg)

**示例 1：**

**输入：**board = [["A","B","C","E"],["S","F","C","S"],["A","D","E","E"]], word = "ABCCED"  
**输出：**true

**示例 2：**

**输入：**board = [["a","b"],["c","d"]], word = "abcd"  
**输出：**false

**提示：**

- `m == board.length`
- `n = board[i].length`
- `1 <= m, n <= 6`
- `1 <= word.length <= 15`
- `board` 和 `word` 仅由大小写英文字母组成

### 解题思路

回溯思路，dfs 的时候需要回退状态，用 vis 数组标记的时候，如果当前格子与之前的连不上，要取消已经访问的状态，否则可能出现，饶了一圈回到这个格子，但是却被之前访问的临近格子置为已经访问了

```cpp
class Solution {
    const int dx[4] = {0, 0, -1, 1};
    const int dy[4] = {1, -1, 0, 0};
    int m, n;
public:
    bool exist(vector<vector<char>>& board, string word) {
        m = board.size();
        n = board[0].size();
        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < n; ++j) {
                if (board[i][j] == word[0]) {
                    vector<vector<int>> vis(m, vector<int>(n, 0));
                    if (dfs(board, vis, i, j, word, 1)) {
                        return true;
                    }
                }
            }
        }
        return false;
    }

    bool dfs(vector<vector<char>>& board, vector<vector<int>>& vis, int x, int y, string& word, int id) {
        if (id >= word.size()) {
            return true;
        }
        if (!check(x, y) || vis[x][y] != 0) {
            return false;
        }
        vis[x][y] = 1;
        for (int i = 0; i < 4; ++i) {
            int mx = x + dx[i];
            int my = y + dy[i];
            if (check(mx, my) && vis[mx][my] == 0 && word[id] == board[mx][my]) {
                if (dfs(board, vis, mx, my, word, id + 1)) {
                    return true;
                }
            }
        }
        vis[x][y] = 0; // 需要回溯
        return false;
    }

    bool check(int x, int y) {
        return x >= 0 && x < m && y >= 0 && y < n;
    }
};
```