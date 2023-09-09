## [剑指 Offer 51. 数组中的逆序对](https://leetcode.cn/problems/shu-zu-zhong-de-ni-xu-dui-lcof/)

在数组中的两个数字，如果前面一个数字大于后面的数字，则这两个数字组成一个逆序对。输入一个数组，求出这个数组中的逆序对的总数。

**示例 1:**

**输入**: [7,5,6,4]
**输出**: 5

**限制：** `0 <= 数组长度 <= 50000`

## 解题思路

离散化树状数组（详细内容参考其他地方吧），这里只是提供一下模版。

```cpp
class Solution {
public:
    int reversePairs(vector<int>& nums) {
        int n = nums.size();
        int res = 0;
        auto mp = discrete(nums);
        tree.resize(n + 1);
        for (int i = 0; i < n; ++i) {
            int pos = mp[nums[i]];
            res += i - query(pos);
            add(pos, 1);
        }
        return res;
    }
	// 离散化
    unordered_map<int, int> discrete(vector<int> &nums) {
        set<int> st(nums.begin(), nums.end());
        unordered_map<int, int> mp;
        int id = 0;
        for (auto num : st) {
            mp[num] = ++id;
        }
        return mp;
    }
private:
    vector<int> tree;
    // 查询前n项和
    int query(int n) { 
        int ans = 0;
        // 下一个左邻区间和结点下标为i -= lowbit(i)
        for(int i = n; i > 0; i -= lowbit(i)){ 
            ans += tree[i];
        }
        return ans;
    }
	
	// 单点修改: nums[k-1] += x (nums[k-1]是 nums 中的第 k 项)
    void add(int k, int x) { 
	    // 下一个区间和结点下标为i += lowbit(i)
        for(int i = k; i < tree.size(); i += lowbit(i)){
            tree[i] += x; // 包含第k项的区间都加上x
        }
    }

    int lowbit(int i) {
        return i & -i;
    }
};
```