## [剑指 Offer 58 - I. 翻转单词顺序](https://leetcode.cn/problems/fan-zhuan-dan-ci-shun-xu-lcof/)

输入一个英文句子，翻转句子中单词的顺序，但单词内字符的顺序不变。为简单起见，标点符号和普通字母一样处理。例如输入字符串 "I am a student. "，则输出 "student. a am I"。

**示例 1：**

**输入:** "`the sky is blue`"  
**输出:** "`blue is sky the`"

**示例 2：**

**输入:** "  hello world!  "  
**输出:** "world! hello"  
**解释:** 输入字符串可以在前面或者后面包含多余的空格，但是反转后的字符不能包括。

**示例 3：**

**输入:** "a good   example"  
**输出:** "example good a"  
**解释:** 如果两个单词间有多余的空格，将反转后单词间的空格减少到只含一个。

**说明：**

- 无空格字符构成一个单词。
- 输入字符串可以在前面或者后面包含多余的空格，但是反转后的字符不能包括。
- 如果两个单词间有多余的空格，将反转后单词间的空格减少到只含一个。

### 解题思路

最开始没注意到有多余空格的存在，思路是利用双指针将每个单词进行反转，然后再整体反转一下即可；

有多余空格需要预处理一下，预处理的思路是用一个标记位 flag，只有当 falg 为 false 的时候才会在遇见空格的时候加上一个空格，什么时候为 false 呢？就是在空格前遇见其他字符的时候，标记位置为 false，最后需要特殊判断一下末尾的空格；

（感觉这样写起来，看着还挺复杂的）

```cpp
class Solution {
public:
    string reverseWords(string s) {
        int n = s.length();
        string str;
        bool flag = true;
        for (int i = 0; i < n; ++i) {
            if (s[i] != ' ') {
                str.push_back(s[i]);
                flag = false;
            } else {
                if (!flag) {
                    str.push_back(' ');
                    flag = true;
                }
            }
        }
        if (str.back() == ' ') {
            str.pop_back();
        }
        int i = 0;
        n = str.length();
        while (i < n) {
            int j = i;
            while (j < n && str[j] != ' ') {
                j++;
            }
            reverse(str, i, j - 1);
            i = j + 1;
        }
        reverse(str, 0, n - 1);
        return str;
    }

    void reverse(string &s, int lo, int hi) {
        while (lo < hi) {
            swap(s[lo], s[hi]);
            lo++; hi--;
        }
    }    
};
```