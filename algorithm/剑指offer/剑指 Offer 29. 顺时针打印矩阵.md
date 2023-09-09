## [剑指 Offer 29. 顺时针打印矩阵](https://leetcode.cn/problems/shun-shi-zhen-da-yin-ju-zhen-lcof/)

输入一个矩阵，按照从外向里以顺时针的顺序依次打印出每一个数字。

**示例 1：**

**输入：**matrix = [[1,2,3],[4,5,6],[7,8,9]]  
**输出：**[1,2,3,6,9,8,7,4,5]

**示例 2：**

**输入：**matrix = [[1,2,3,4],[5,6,7,8],[9,10,11,12]]  
**输出：**[1,2,3,4,8,12,11,10,9,5,6,7]

**限制：**

- `0 <= matrix.length <= 100`
- `0 <= matrix[i].length <= 100`

### 解题思路

这题思维难度并不高，但是想写对代码还是稍微有点难度，可能总是遇到越界情况；

考虑给矩阵上下左右设个边界，每次遍历完一行/一列，就调整边界，发现上下边界或者左右边界交叉跳出循环，下面是代码实现：

```cpp
class Solution {
public:
    vector<int> spiralOrder(vector<vector<int>>& matrix) {
        int m = matrix.size();
        if (m == 0) return {};
        int n = matrix[0].size();
        if (n == 0) return {};
        int l = 0, r = n - 1, t = 0, b = m - 1;
        vector<int> res;
        res.reserve(m * n);
        while (l <= r && t <= b) {
            for (int j = l; j <= r; ++j) {
                res.emplace_back(matrix[t][j]);
            }
            if (++t > b) break;
            for (int i = t; i <= b; ++i) {
                res.emplace_back(matrix[i][r]);
            }
            if (--r < l) break;
            for (int j = r; j >= l; --j) {
                res.emplace_back(matrix[b][j]);
            }
            if (--b < t) break;
            for (int i = b; i >= t; --i) {
                res.emplace_back(matrix[i][l]);
            }
            if (++l > r) break;
        }
        return res;
    }
};
```