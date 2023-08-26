## [剑指 Offer 16. 数值的整数次方](https://leetcode.cn/problems/shu-zhi-de-zheng-shu-ci-fang-lcof/)

实现 [pow(_x_, _n_)](https://www.cplusplus.com/reference/valarray/pow/) ，即计算 x 的 n 次幂函数（即，xn）。不得使用库函数，同时不需要考虑大数问题。

**示例 1：**

**输入：**x = 2.00000, n = 10  
**输出：**1024.00000

**示例 2：**

**输入：**x = 2.10000, n = 3  
**输出：**9.26100

**示例 3：**

**输入：**x = 2.00000, n = -2  
**输出：**0.25000  
**解释：**2-2 = 1/22 = 1/4 = 0.25

**提示：**

- `-100.0 < x < 100.0`
- `-231 <= n <= 231-1`
- `n` 是一个整数
- 要么 `x` 不为零，要么 `n > 0` 。
- `-104 <= xn <= 104`

## 解题思路

快速幂模版题，注意负数的时候可能会溢出

```cpp
class Solution {
public:
    double myPow(double x, int n) {
		double res = 1.0;
        long long k = n;
        if (k < 0) {
            k = -k;
            x = 1 / x;
        }
        while (k) {
            if (k & 1) res = res * x;
            x = x * x;
            k >>= 1;
        }
        return res;
    }
};
```