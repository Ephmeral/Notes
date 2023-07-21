## [剑指 Offer 03. 数组中重复的数字](https://leetcode.cn/problems/shu-zu-zhong-zhong-fu-de-shu-zi-lcof/)

找出数组中重复的数字。

  
在一个长度为 n 的数组 nums 里的所有数字都在 0～n-1 的范围内。数组中某些数字是重复的，但不知道有几个数字重复了，也不知道每个数字重复了几次。请找出数组中任意一个重复的数字。

**示例 1：**

**输入：**  
[2, 3, 1, 0, 2, 5, 3]  
**输出：**2 或 3

**限制：**

`2 <= n <= 100000`


### 解题思路

最简单的想法是利用哈希表记录已经遍历过的数字，当再次遇到哈希表中存在的元素时，说明此时找到了重复的元素，返回即可

```cpp
class Solution {
public:
    int findRepeatNumber(vector<int>& nums) {
        unordered_set<int> st;
        for (auto num : nums) {
            if (st.count(num)) {
                return num;
            }
            st.insert(num);
        }
        return -1;
    }
};
```

时间复杂度：O(n)  
空间复杂度：O(n)

优化空间复杂度的方法是考虑原地交换，题目中说了所有数字在 0 ~ n-1 范围内，所以利用数组本身也可以看成哈希表，交换的时候：
- 如果 $nums[i] == i$，说明已经在正确位置上了，i++；
- 如果 $nums[i] == nums[nums[i]]$，说明找到一个重复的数字；
- 否则交换 $nums[i]$ 和 $nums[nums[i]]$

```cpp
class Solution {
public:
    int findRepeatNumber(vector<int>& nums) {
        for (int i = 0; i < nums.size(); ) {
            if (nums[i] == i) {
                i++;
                continue;
            }
            if (nums[i] == nums[nums[i]]) {
                return nums[i];
            }
            swap(nums[i], nums[nums[i]]);
        }
        return -1;
    }
};
```

时间复杂度：O(n)
空间复杂度：O(1)