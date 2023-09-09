## [剑指 Offer 59 - I. 滑动窗口的最大值](https://leetcode.cn/problems/hua-dong-chuang-kou-de-zui-da-zhi-lcof/)

给定一个数组 `nums` 和滑动窗口的大小 `k`，请找出所有滑动窗口里的最大值。

**示例:**

```
输入: nums = [1,3,-1,-3,5,3,6,7], 和 k = 3
输出: [3,3,5,5,6,7]  解释: 
  滑动窗口的位置                最大值
---------------               -----
[1  3  -1] -3  5  3  6  7       3
 1 [3  -1  -3] 5  3  6  7       3
 1  3 [-1  -3  5] 3  6  7       5
 1  3  -1 [-3  5  3] 6  7       5
 1  3  -1  -3 [5  3  6] 7       6
 1  3  -1  -3  5 [3  6  7]      7
```

**提示：**

你可以假设 _k_ 总是有效的，在输入数组 **不为空** 的情况下，`1 ≤ k ≤ nums.length`。

### 解题思路

1. 有序哈希表

一种比较容易想到的是利用有序的哈希表，维护区间所有的数，然后根据反向迭代器得到最大值；

```cpp
class Solution {
public:
    vector<int> maxSlidingWindow(vector<int>& nums, int k) {
        vector<int> res;
        map<int, int> mp;
        for (int i = 0; i < k; ++i) {
            mp[nums[i]]++;
        }
        for (int i = k; i < nums.size(); ++i) {
            res.push_back(mp.rbegin()->first);
            mp[nums[i]]++;
            mp[nums[i - k]]--;
            if (mp[nums[i - k]] == 0) {
                mp.erase(nums[i - k]);
            }
        }
        res.push_back(mp.rbegin()->first);
        return res;
    }
};
```

这种方法用优先队列好像也可以，这里没有尝试，另外一种比较好的思路是利用单调队列。

2. 单调队列

单调队列最终实现的目的是队列 front 为最大值，队尾为最小值。具体维护的时候，插入数据 x 的时候，如果 x 比队尾元素大，将队尾的数字移除，直到遇见队尾比 x 大的元素，然后 x 插入队尾；移除头部数据的时候，只有当待移除的 x 和队头元素相等的时候，才移除队头元素，否则可以认为 x 之前已经在插入的时候被移除了；

```cpp
class Queue {
public:
    void push(int x) {
        while (!que.empty()) {
            if (que.back() < x) {
                que.pop_back();
            } else {
                break;
            }
        }
        que.push_back(x);
    }

    void pop(int x) {
        if (!que.empty() && que.front() == x) {
            que.pop_front();
        }
    }

    int top() {
        return que.front();
    }

private:
    deque<int> que;
};

class Solution {
public:
    vector<int> maxSlidingWindow(vector<int>& nums, int k) {
        vector<int> res;
        Queue que;
        for (int i = 0; i < k; ++i) {
            que.push(nums[i]);
        }
        res.push_back(que.top());
        for (int i = k; i < nums.size(); ++i) {
            que.pop(nums[i - k]);
            que.push(nums[i]);
            res.push_back(que.top());
        }
        return res;
    }
};
```