数组中有一个数字出现的次数超过数组长度的一半，请找出这个数字。

你可以假设数组是非空的，并且给定的数组总是存在多数元素。

**示例 1:**

```
输入: [1, 2, 3, 2, 2, 2, 5, 4, 2]
输出: 2
```

**限制：**

$1 <= 数组长度 <= 50000$

注意：本题与主站 169 题相同：[https://leetcode-cn.com/problems/majority-element/](https://leetcode-cn.com/problems/majority-element/)

## 解题思路

暴力一点的做法是用哈希表，直接能对每个数字进行计数，然后找到大于一半的数字即可

```cpp
class Solution {
public:
    int majorityElement(vector<int>& nums) {
        unordered_map<int, int> mp;
        for (auto num : nums) mp[num]++;
        for (auto &[num, cnt] : mp) {
            if (cnt > nums.size() / 2) {
                return num;
            }
        }
        return -1;
    }
};
```

如果要求时间复杂度 O(n)，空间复杂度 O(1) 的话，考虑每次移走 2 个不一样的数字，所有操作完成之后剩余的数一定是最多的那个。

为什么？因为即使每次都会移走最多的那个数字，最后也还是剩下的更多。算法实现上，可以用 res 表示前面次数多的那个，cnt 用来记录这个数字有多少个
- 如果当前 num 等于 res 的话，说明是重复的数字，cnt + 1
- 如果 num 不等于 res 的话，这个时候将 res 和 num 都去除，这两个是不同的数字；
- 当 cnt 小于等于 0 的时候，说明之前没有剩余的数可以去除，将当前的数字作为最多的数字保留；

```cpp
class Solution {
public:
    int majorityElement(vector<int>& nums) {
        int res = -1, cnt = 0;
        for (int num : nums) {
            if (num == res) ++cnt;
            else if (--cnt < 0) {
                res = num;
                cnt = 1;
            }
        }
        return res;
    }
};
```