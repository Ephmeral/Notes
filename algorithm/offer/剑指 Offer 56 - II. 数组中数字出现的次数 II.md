## [剑指 Offer 56 - II. 数组中数字出现的次数 II](https://leetcode.cn/problems/shu-zu-zhong-shu-zi-chu-xian-de-ci-shu-ii-lcof/)

在一个数组 `nums` 中除一个数字只出现一次之外，其他数字都出现了三次。请找出那个只出现一次的数字。

**示例 1：**

**输入：**nums = [3,4,3,3]  
**输出：**4

**示例 2：**

**输入：**nums = [9,1,7,9,7,9,7]  
**输出：**1

**限制：**

- `1 <= nums.length <= 10000`
- `1 <= nums[i] < 2^31`

## 解题思路

哈希表暴力遍历（比较不讲武德）

```cpp
class Solution {
public:
    int singleNumber(vector<int>& nums) {
        unordered_map<int, int> mp;
        for (auto num : nums) {
            mp[num]++;
        }
        for (auto& [key, cnt] : mp) {
            if (cnt == 1) {
                return key;
            }
        }
        return -1;
    }
};
```

考虑每个二进制位，因为除了只出现一次的数外，其他都出现 3 次，所以这个二进制中 1 的个数肯定是 3 的倍数，如果有多的说明单独的那一个数这个二进制位也是有的，所以根据此来计算得到最终的结果。可以用一个大小 32 的数组，记录每一位 1 的个数。

```cpp
class Solution {
public:
    int singleNumber(vector<int>& nums) {
        int cnt[32] = {0};
        for (auto num : nums) {
            for (int i = 0; i < 31; ++i) {
                cnt[i] += (num >> i) & 1; // 记录第 i 位 1 的个数
            }
        }
        int res = 0;
        for (int i = 0; i < 31; ++i) {
            if (cnt[i] % 3 == 1) { // 如果不是3的倍数，说明单独的数这位也是 1
                res |= (1 << i); 
            }
        }
        return res;
    }
};
```