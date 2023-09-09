## [剑指 Offer 57 - II. 和为s的连续正数序列](https://leetcode.cn/problems/he-wei-sde-lian-xu-zheng-shu-xu-lie-lcof/)

输入一个正整数 `target` ，输出所有和为 `target` 的连续正整数序列（至少含有两个数）。

序列内的数字由小到大排列，不同序列按照首个数字从小到大排列。

**示例 1：**  
**输入：** target = 9  
**输出：**`[[2,3,4],[4,5]]`

**示例 2：**  
**输入：** target = 15  
**输出：**`[[1,2,3,4,5],[4,5,6],[7,8]]`

**限制：**
- `1 <= target <= 10^5`

## 解题思路

考虑用数学方法，需要计算 i + (i + 1) + (i + 2) + ... + (i + m) == target，如果 m 是个整数解的话，说明区间 [i, i + m] 是能构成 target 的，符合题目要求。

进一步上面公式推导为 i * (m + 1) + (m + 1) * m / 2 == target，即 $m^2 + (2 * i + 1) * m + 2i - 2 target == 0$

利用求根公式可知 $m = \frac{\sqrt{(2i + 1)^2 + 8(t - i)} - (2i + 1)}{2}$ (另外一个解为负数，直接去除)

根据求根公式需要根号下的结果为整数，同时分子要能够整除 2，这样就能求出 m 了，最后将符合条件的区间返回即可（还要考虑整数溢出的情况，可以转为 long long 类型）

```cpp
class Solution {
public:
    vector<vector<int>> findContinuousSequence(int target) {
        vector<vector<int>> res;
        for (int i = 1; i <= (target - 1) / 2; ++i) {
            int left = (int)sqrt(1LL * (2 * i + 1) * (2 * i + 1) + 8 * (target - i));
            if (1LL * left * left == 1LL * (2 * i + 1) * (2 * i + 1) + 8 * (target - i)) {
                int m = left - (2 * i + 1);
                if (m % 2 == 0) {
                    m /= 2;
                    vector<int> tmp;
                    for (int j = i; j <= m + i; ++j) {
                        tmp.push_back(j);
                    }
                    res.push_back(tmp);
                }
            }
        }
        return res;
    }
};
```

第二种方法：对于 target =15 ，结果为 `[[1,2,3,4,5],[4,5,6],[7,8]]`，可以发现每个区间的右边界一定比前一个区间的右边界大，所以用 i, j 分别指向左右边界：
- 当区间和 sum < target 的时候移动右边界，即 j++；
- 当区间和 sum > target 的时候移动左边界，即 i++；
- 当 sum == target 的时候即找到一组答案，将区间 [i, j] 的值放到答案中，然后移动左边界；

```cpp
class Solution {
public:
    vector<vector<int>> findContinuousSequence(int target) {
        vector<vector<int>> res;
        int i = 1, j = 1, n = (target - 1) / 2;
        int sum = 0;
        while (i <= n) {
            if (sum < target) {
                sum += j;
                ++j;               
            } else if (sum > target) {
                sum -= i;
                ++i;
            } else {
                vector<int> tmp;
                for (int k = i; k < j; ++k) {
                    tmp.push_back(k);
                }
                res.push_back(tmp);
                sum -= i;
                ++i;
            }
        }
        return res;
    }
};
```