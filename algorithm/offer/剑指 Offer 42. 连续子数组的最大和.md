## [剑指 Offer 42. 连续子数组的最大和](https://leetcode.cn/problems/lian-xu-zi-shu-zu-de-zui-da-he-lcof/)

输入一个整型数组，数组中的一个或连续多个整数组成一个子数组。求所有子数组的和的最大值。

要求时间复杂度为 O(n)。

**示例 1:**

**输入:** nums = [-2,1,-3,4,-1,2,1,-5,4]  
**输出:** 6  
**解释:** 连续子数组 [4,-1,2,1] 的和最大，为 6。

**提示：**

- `1 <= arr.length <= 10^5`
- `-100 <= arr[i] <= 100`

## 解题思路

用动态规划思想：
- 以 dp[i] 表示以 i 结尾，能够组成的连续子数组能的最大值
- 如果 dp[i - 1] < 0，则不加上前面的，nums[i] 单独作为一个数组，因为这样会使值变得更小；
- 如果 dp[i - 1] >= 0，则 dp[i] = dp[i - 1] + nums[i]，即 nums[i] 结尾并且加上前面的最大值；


```cpp
class Solution {
public:
    int maxSubArray(vector<int>& nums) {
        int n = nums.size();
        vector<int> dp(nums.begin(), nums.end());
        for (int i = 1; i < n; ++i) {
            if (dp[i - 1] >= 0) {
                dp[i] += dp[i - 1];
            } else {
                // dp[i] = 0;
            }
        }
        return *max_element(dp.begin(), dp.end());
    }
};
```