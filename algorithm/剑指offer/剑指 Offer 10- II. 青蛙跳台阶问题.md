## [剑指 Offer 10- II. 青蛙跳台阶问题](https://leetcode.cn/problems/qing-wa-tiao-tai-jie-wen-ti-lcof/)

一只青蛙一次可以跳上 1 级台阶，也可以跳上 2 级台阶。求该青蛙跳上一个 `n` 级的台阶总共有多少种跳法。

答案需要取模 1e9+7（1000000007），如计算初始结果为：1000000008，请返回 1。

**示例 1：**

**输入：**n = 2  
**输出：**2

**示例 2：**

**输入：**n = 7  
**输出：**21

**示例 3：**

**输入：**n = 0  
**输出：**1

**提示：**

- `0 <= n <= 100`

## 解题思路

和 10-I 几乎一模一样，除了初始值 `f[0] = 1` 外，其他都一样

```cpp
class Solution {
    long long f[1000];
    const int mod = 1e9 + 7;
public:
    int numWays(int n) {
        f[0] = f[1] = 1;
        for (int i = 2; i <= n; ++i) {
            f[i] = (f[i - 1] + f[i - 2]) % mod;
        }
        return f[n];
    }
};
```