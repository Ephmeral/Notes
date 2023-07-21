## [剑指 Offer 53 - II. 0～n-1中缺失的数字](https://leetcode.cn/problems/que-shi-de-shu-zi-lcof/)

一个长度为 n-1 的递增排序数组中的所有数字都是唯一的，并且每个数字都在范围 0～n-1 之内。在范围 0～n-1 内的 n 个数字中有且只有一个数字不在该数组中，请找出这个数字。

**示例 1:**

**输入:** [0,1,3]  
**输出:** 2

**示例 2:**

**输入:** [0,1,2,3,4,5,6,7,9]  
**输出:** 8

**限制：**

`1 <= 数组长度 <= 10000`


### 解题思路

有两种情况：
- 0 ~ n - 1 少了中间某个值，此时 $nums[i] \neq i$；
- 少了最后一个值 n，此时数组中其他数字都是对应的；

```cpp
class Solution {
public:
    int missingNumber(vector<int>& nums) {
        int cnt = 0;
        for (int i = 0; i < nums.size(); ++i) {
	        // 1. 少了中间某个值
            if (i != nums[i]) {
                return i;
            }
            ++cnt;
        }
        // 2. 少了最后一个值
        return cnt;
    }
};
```