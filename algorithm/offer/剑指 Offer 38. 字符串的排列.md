## [剑指 Offer 38. 字符串的排列](https://leetcode.cn/problems/zi-fu-chuan-de-pai-lie-lcof/)

输入一个字符串，打印出该字符串中字符的所有排列。

你可以以任意顺序返回这个字符串数组，但里面不能有重复元素。

**示例:**

**输入：**s = "abc"  
**输出：[**"abc","acb","bac","bca","cab","cba"**]**

**限制：**

`1 <= s 的长度 <= 8`

## 解题思路

回溯，每次从任意位置选取，然后回溯下去，注意需要去重，表现为当 i - 1 没有访问，并且 `s[i] == s[i - 1]` 说明此时不需要再选取 `s[i]`，因为 `s[i - 1]` 已经选取过了；

```cpp
class Solution {
public:
    vector<string> permutation(string s) {
        sort(s.begin(), s.end());
        vector<int> vis(s.length(), 0);
        backtrack(s, 0, vis);
        return res;
    }

private:
    vector<string> res;
    string path;
    void backtrack(string &s, int id, vector<int> &vis) {
        if (id == s.length()) {
            res.push_back(path);
            return;
        }
        for (int i = 0; i < s.length(); ++i) {
            if (!vis[i]) {
                if (i > 0 && vis[i - 1] == false && s[i] == s[i - 1]) continue;
                vis[i] = true;
                path.push_back(s[i]);
                backtrack(s, id + 1, vis);
                path.pop_back();
                vis[i] = false;
            }
        }
    }
};
```

下一个排列：C++ 的 `next_permutation` 函数可以快速得到字典序的下一个排列，这里模拟了一下

```cpp
class Solution {
public:
    bool nextPermutation(string& s) {
        int i = s.size() - 2;
        while (i >= 0 && s[i] >= s[i + 1]) {
            i--;
        }
        if (i < 0) {
            return false;
        }
        int j = s.size() - 1;
        while (j >= 0 && s[i] >= s[j]) {
            j--;
        }
        swap(s[i], s[j]);
        reverse(s.begin() + i + 1, s.end());
        return true;
    }

    vector<string> permutation(string s) {
        vector<string> ret;
        sort(s.begin(), s.end());
        do {
            ret.push_back(s);
        } while (nextPermutation(s));
        return ret;
    }
};
```