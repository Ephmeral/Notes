## [剑指 Offer 64. 求1+2+…+n](https://leetcode.cn/problems/qiu-12n-lcof/)

求 `1+2+...+n` ，要求不能使用乘除法、for、while、if、else、switch、case 等关键字及条件判断语句（A?B:C）。

**示例 1：**

**输入:** n = 3  
**输出:** 6

**示例 2：**

**输入:** n = 9  
**输出:** 45

**限制：**

- `1 <= n <= 10000`

## 解题思路

感觉这题目有点 ntr，用&&短路计算的特点，递归求解

```cpp
class Solution {
public:
    int sumNums(int n) {
        n && (n += sumNums(n - 1));
        return n;
    }
};
```