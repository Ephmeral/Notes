## [剑指 Offer 61. 扑克牌中的顺子](https://leetcode.cn/problems/bu-ke-pai-zhong-de-shun-zi-lcof/)

从**若干副扑克牌**中随机抽 `5` 张牌，判断是不是一个顺子，即这 5 张牌是不是连续的。2～10 为数字本身，A 为 1，J 为 11，Q 为 12，K 为 13，而大、小王为 0 ，可以看成任意数字。A 不能视为 14。

**示例 1:**

**输入:** [1,2,3,4,5]  
**输出:** True

**示例 2:**

**输入:** [0,0,1,2,5]  
**输出:** True

**限制：**

数组长度为 5 

数组的数取值为 [0, 13] .

## 解题思路

暴力一点的做法是，用个数组模拟哈希桶，存放下所有数字，另外记录 0 的个数，然后按照区间 5 来统计缺少的牌数量是否和大小王数量一致

```cpp
class Solution {
public:
    bool isStraight(vector<int>& nums) {
        int zero = 0;
        int mp[15] = {0};
        for (auto num : nums) {
            if (num == 0) zero++;
            mp[num]++;
            // 除了大小王有重复的，直接返回false
            if (num && mp[num] > 1) return false; 
        }
        for (int i = 1; i <= 8; ++i) {
            int cnt = 0; // 记录缺少的牌
            for (int j = i; j < i + 5; ++j) {
                if (mp[j] == 0) {
                    cnt++;
                }
            }
            // 和大小王数量相同，可以构成顺子
            if (cnt == zero) return true;
        }
        return false;
    }
};
```

优化一下的话，可以直接判断除大小王的最大牌和最小牌差值是否小于 5

```cpp
class Solution {
public:
    bool isStraight(vector<int>& nums) {
        int zero = 0;
        int mp[15] = {0};
        int mx = -1, mi = 20;
        for (auto num : nums) {
            if (num == 0) zero++;
            else {
                mp[num]++;
                // 除了大小王有重复的，直接返回false
                if (num && mp[num] > 1) return false;
                mx = max(mx, num);
                mi = min(mi, num);
            }
        }
        return mx - mi < 5;
    }
};
```