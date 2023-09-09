## [剑指 Offer 17. 打印从1到最大的n位数](https://leetcode.cn/problems/da-yin-cong-1dao-zui-da-de-nwei-shu-lcof/)

输入数字 `n`，按顺序打印出从 1 到最大的 n 位十进制数。比如输入 3，则打印出 1、2、3 一直到最大的 3 位数 999。

**示例 1:**

**输入:** n = 1  
**输出:** [1,2,3,4,5,6,7,8,9]

说明：

- 用返回一个整数列表来代替打印
- n 为正整数

## 解题思路

这题好像和书上有出入，题目不考虑大数是可以直接通过的，如果考虑大数的话，需要用字符串表示

```cpp
class Solution {
public:
    vector<int> printNumbers(int n) {
        int mx = 0;
        while (n) {
            mx = mx * 10 + 9;
            --n;
        }
        vector<int> res;
        for (int i = 1; i <= mx; i++) {
            res.emplace_back(i);
        }
        return res;
    }
};
```

考虑大数的话，并且假设返回的是 `vector<string`，用回溯的思路解决：

```cpp
class Solution {
    vector<string> res;
    string cur;
    char NUM[10] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
    
    // 生成长度为 len 的数字，正在确定第x位（从左往右）
    void dfs(int x, int len) {
        if(x == len) {
            res.push_back(cur);
            return;
        }
        int start = x == 0? 1 : 0; // X=0表示左边第一位数字，不能为0
        for(int i = start; i < 10; i++) {
            cur.push_back(NUM[i]); // 确定本位数字
            dfs(x + 1, len); // 确定下一位数字
            cur.pop_back(); // 删除本位数字
        }
    }
public:
    vector<string> printNumbers(int n) {
        for(int i = 1; i <= n; i++) // 数字长度：1~n
            dfs(0, i);
        return res;
    }
};
```