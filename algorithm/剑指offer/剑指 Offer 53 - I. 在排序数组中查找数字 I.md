## [剑指 Offer 53 - I. 在排序数组中查找数字 I](https://leetcode.cn/problems/zai-pai-xu-shu-zu-zhong-cha-zhao-shu-zi-lcof/)


统计一个数字在排序数组中出现的次数。

**示例 1:**

**输入:** nums = [`5,7,7,8,8,10]`, target = 8  
**输出:** 2

**示例 2:**

**输入:** nums = [`5,7,7,8,8,10]`, target = 6  
**输出:** 0

**提示：**

- `0 <= nums.length <= 105`
- `-109 <= nums[i] <= 109`
- `nums` 是一个非递减数组
- `-109 <= target <= 109`

### 解题思路

直观的暴力，就不多说了

考虑题目是排序数组，可以利用二分法来找到上下边界，直接调用 C++ 的 lower_bound 函数找到大于等于 target 的位置，upper_bound 找到大于 target 的位置，二者做差即为结果

```cpp
class Solution {
public:
    int search(vector<int>& nums, int target) {
        int lo = lower_bound(nums.begin(), nums.end(), target) - nums.begin();
        int hi = upper_bound(nums.begin(), nums.end(), target) - nums.begin();
        return hi - lo;
    }
};
```

稍微复杂点，就是自己实现二分，我这里的 lower_bound 是查找大于等于 target 的位置，upper_bound 是查找小于等于 target 的位置，注意特殊情况：

```cpp
class Solution {
public:
    int search(vector<int>& nums, int target) {
        if (nums.size() == 0) {
            return 0;
        }
        
        int lo = lower_bound(nums, target);
        int hi = upper_bound(nums, target);
        if (lo == -1 || hi == -1) {
            return 0;
        }
        return hi - lo + 1;
    }

    int lower_bound(vector<int> &nums, int target) {
        int lo = 0, hi = nums.size() - 1;
        while (lo < hi) {
            int mi = lo + ((hi - lo) >> 1);
            if (nums[mi] < target) {
                lo = mi + 1;
            } else {
                hi = mi;
            }
        }
        return nums[lo] == target ? lo : -1;
    }

    int upper_bound(vector<int> &nums, int target) {
        int lo = 0, hi = nums.size() - 1;
        while (lo < hi) {
            int mi = lo + ((hi - lo + 1) >> 1);
            if (nums[mi] <= target) {
                lo = mi;
            } else {
                hi = mi - 1;
            }
        }
        return nums[lo] == target ? lo : -1;
    }
};
```