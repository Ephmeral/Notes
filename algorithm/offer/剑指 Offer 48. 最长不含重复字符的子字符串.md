## [剑指 Offer 48. 最长不含重复字符的子字符串](https://leetcode.cn/problems/zui-chang-bu-han-zhong-fu-zi-fu-de-zi-zi-fu-chuan-lcof/)

请从字符串中找出一个最长的不包含重复字符的子字符串，计算该最长子字符串的长度。

**示例 1:**

**输入:** "abcabcbb"  
**输出:** 3  
**解释:** 因为无重复字符的最长子串是 `"abc"，所以其` 长度为 3。

**示例 2:**

**输入:** "bbbbb"  
**输出:** 1  
**解释:** 因为无重复字符的最长子串是 `"b"`，所以其长度为 1。

**示例 3:**

**输入:** "pwwkew"  
**输出:** 3  
**解释:** 因为无重复字符的最长子串是 `"wke"`，所以其长度为 3。  
     请注意，你的答案必须是 **子串** 的长度，`"pwke"` 是一个 _ 子序列，_ 不是子串。

提示：
- `s.length <= 40000`

## 解题思路

滑动窗口，用哈希表维护窗口中的字符，枚举右端点，插入右端点的元素之前，需要移动左端点直到哈希表中没有重复元素出现

```cpp
class Solution {
public:
    int lengthOfLongestSubstring(string s) {
        set<char> st;
        int res = 0, j = 0;
        for (int i = 0; i < s.length(); ++i) {
            while (j < i && st.count(s[i])) {
                st.erase(s[j]);
                ++j;
            }
            st.insert(s[i]);
            res = max(res, i - j + 1);
        }
        return res;
    }
};
```