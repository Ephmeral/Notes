## [剑指 Offer 46. 把数字翻译成字符串](https://leetcode.cn/problems/ba-shu-zi-fan-yi-cheng-zi-fu-chuan-lcof/)

给定一个数字，我们按照如下规则把它翻译为字符串：0 翻译成 “a” ，1 翻译成 “b”，……，11 翻译成 “l”，……，25 翻译成 “z”。一个数字可能有多个翻译。请编程实现一个函数，用来计算一个数字有多少种不同的翻译方法。

**示例 1:**

**输入:** 12258  
**输出:** `5`  
**解释:** 12258 有 5 种不同的翻译，分别是 "bccfi", "bwfi", "bczi", "mcfi" 和 "mzi"

**提示：**

- `0 <= num < 231`

## 解题思路

这题目相当于是判断一个数字能有多少种划分方法，划分的每个数范围在 0 - 25，这样的话考虑使用动态规划。将数字转为字符串形式，长度不会很长，以 dp[i] 表示以 i 结尾的字符串能有多少种翻译的方法。
- 转移方程：dp[i] 有两种方法组成
	- 一种是将 s[i] 单独作为一个数，则 dp[i] = dp[i - 1]
	- 另外是 s[i - 1] 和 s[i] 能够组成一个合法的数（这里合法指的是 s[i - 1] 不为 0，且两个数的大小在 0 - 25 之间），则 dp[i] = dp[i - 2]
- 初始化：考虑 dp 数组大小多加 1，dp[0] 表示什么都没选，可以看作一种方法，dp[1] 表示选第一个数字，也是一种方法，即 `dp[0] = dp[1] = 1`

```cpp
class Solution {
public:
    int translateNum(int num) {
        string s = to_string(num);
        int n = s.length();
        vector<int> dp(n + 1, 0);
        dp[0] = dp[1] = 1;
        for (int i = 2; i <= n; ++i) {
            char cur = s[i - 1];
            char pre = s[i - 2];
            if (((pre - '0') * 10 + (cur - '0') <= 25) && (pre != '0')) {
                dp[i] += dp[i - 2];
			}
            dp[i] += dp[i - 1];
        }
        return dp[n];
    }
};
```