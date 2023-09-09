## [剑指 Offer 21. 调整数组顺序使奇数位于偶数前面](https://leetcode.cn/problems/diao-zheng-shu-zu-shun-xu-shi-qi-shu-wei-yu-ou-shu-qian-mian-lcof/)

输入一个整数数组，实现一个函数来调整该数组中数字的顺序，使得所有奇数在数组的前半部分，所有偶数在数组的后半部分。

**示例：**

**输入：**nums = [1,2,3,4]  
**输出：**[1,3,2,4]  
**注：**[3,1,2,4] 也是正确的答案之一。

**提示：**

1. `0 <= nums.length <= 50000`
2. `0 <= nums[i] <= 10000`

### 解题思路

1. 排序

根据奇偶性排序，奇数在前，偶数在后，相同奇偶性的话依据大小排序（这个无要求）

```cpp
class Solution {
public:
    vector<int> exchange(vector<int>& nums) {
        sort(nums.begin(), nums.end(), [](int a, int b) -> bool {
            // 位运算得到最后一位，不同的话说明有奇偶性不一样
            if ((a & 1) != (b & 1)) {
                // 返回a & 1，如果为0说明a是偶数隐式转为false排在后面
                // 如果为1说明a是奇数，隐式转为true排在前面
                return a & 1;
            }
            return a < b;
        });
        return nums;
    }
};
```

2. 双指针

- 两个指针 lo 和 hi 分别指向数组下标 0 和 `nums.size() - 1`;
- 移动指针的时候，根据奇偶性判断：
	- 如果遇见 `nums[lo]` 为奇数，`lo++`
	- 如果 `nums[hi]` 为偶数 `hi--`；
	- 如果 `nums[lo]` 为偶数，且 `nums[hi]` 为奇数，则两者交换；
- 直到两个指针判断，循环结束；

```cpp
class Solution {
public:
    vector<int> exchange(vector<int>& nums) {
        int lo = 0, hi = nums.size() - 1;
        while (lo < hi) {
            if (nums[lo] % 2 == 0 && nums[hi] % 2 == 1) {
                swap(nums[lo], nums[hi]);
                lo++; hi--;
            }
            if (nums[lo] % 2 == 1) {
                lo++;
            }
            if (nums[hi] % 2 == 0) {
                hi--;
            }
        }
        return nums;
    }
};
```