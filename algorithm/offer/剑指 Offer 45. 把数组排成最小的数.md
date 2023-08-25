## [剑指 Offer 45. 把数组排成最小的数](https://leetcode.cn/problems/ba-shu-zu-pai-cheng-zui-xiao-de-shu-lcof/)

输入一个非负整数数组，把数组里所有数字拼接起来排成一个数，打印能拼接出的所有数字中最小的一个。

**示例 1:**

**输入:** `[10,2]`  
**输出:** "`102"`

**示例 2:**

**输入:** `[3,30,34,5,9]`  
**输出:** "`3033459"`

**提示:**
- `0 < nums.length <= 100`

**说明:**
- 输出结果可能非常大，所以你需要返回一个字符串而不是整数
- 拼接起来的数字可能会有前导 0，最后结果不需要去掉前导 0

## 解题思路

这题考虑直接用排序，将数字转为字符串，然后排序的时候拼接两个数字 x, y，排序规则 x + y < y + x。

```cpp
class Solution {
public:
    string minNumber(vector<int>& nums) {
        vector<string> vec;
        vec.reserve(nums.size());
        for (int &num : nums) {
            vec.emplace_back(to_string(num));
        }
        sort(vec.begin(), vec.end(), [](auto &s1, auto &s2) {
            return s1 + s2 < s2 + s1;
        });
        string res;
        for (auto &s : vec) {
            res += s;
        }
        return res;
    }
};
```