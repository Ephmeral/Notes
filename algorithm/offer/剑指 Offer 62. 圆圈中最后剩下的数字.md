## [剑指 Offer 62. 圆圈中最后剩下的数字](https://leetcode.cn/problems/yuan-quan-zhong-zui-hou-sheng-xia-de-shu-zi-lcof/)

0,1,···,n-1这n个数字排成一个圆圈，从数字0开始，每次从这个圆圈里删除第m个数字（删除后从下一个数字开始计数）。求出这个圆圈里剩下的最后一个数字。

例如，0、1、2、3、4这5个数字组成一个圆圈，从数字0开始每次删除第3个数字，则删除的前4个数字依次是2、0、4、1，因此最后剩下的数字是3。

**示例 1：**

**输入:** n = 5, m = 3
**输出:** 3

**示例 2：**

**输入:** n = 10, m = 17
**输出:** 2

**限制：**
- `1 <= n <= 10^5`
- `1 <= m <= 10^6`

## 解题思路

这题是约瑟夫环问题，感觉有点不明所以，看下下面几个题解吧：

[剑指 Offer 62. 圆圈中最后剩下的数字 - 力扣（LeetCode）](https://leetcode.cn/problems/yuan-quan-zhong-zui-hou-sheng-xia-de-shu-zi-lcof/solutions/177639/javajie-jue-yue-se-fu-huan-wen-ti-gao-su-ni-wei-sh/?envType=study-plan-v2&envId=coding-interviews)
[剑指 Offer 62. 圆圈中最后剩下的数字 - 力扣（LeetCode）](https://leetcode.cn/problems/yuan-quan-zhong-zui-hou-sheng-xia-de-shu-zi-lcof/solutions/607638/jian-zhi-offer-62-yuan-quan-zhong-zui-ho-dcow/?envType=study-plan-v2&envId=coding-interviews)

```cpp
class Solution {
public:
    int lastRemaining(int n, int m) {
        int ans = 0;
        for (int i = 2; i <= n; ++i) {
            ans = (ans + m) % i;
        }
        return ans;
    }
};
```