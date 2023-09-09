## [剑指 Offer 59 - II. 队列的最大值](https://leetcode.cn/problems/dui-lie-de-zui-da-zhi-lcof/)

请定义一个队列并实现函数 `max_value` 得到队列里的最大值，要求函数 `max_value`、`push_back` 和 `pop_front` 的**均摊**时间复杂度都是 O(1)。

若队列为空，`pop_front` 和 `max_value` 需要返回 -1

**示例 1：**

**输入:**  
["MaxQueue","push_back","push_back","max_value","pop_front","max_value"]  
[[],[1],[2],[],[],[]]  
**输出:** [null,null,null,2,1,2]

**示例 2：**

**输入:**  
["MaxQueue","pop_front","max_value"]  
[[],[],[]]  
**输出:** [null,-1,-1]

**限制：**

- `1 <= push_back,pop_front,max_value的总操作数 <= 10000`
- `1 <= value <= 10^5`

### 解题思路

这题可以直接复用上题 [剑指 Offer 59 - I. 滑动窗口的最大值](./剑指%20Offer%2059%20-%20I.%20滑动窗口的最大值.md) 实现的单调队列

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

    bool empty() {
        return que.empty();
    }

private:
    deque<int> que;
};

class MaxQueue {
public:
    MaxQueue() {

    }
    
    int max_value() {
        return mx_que.empty() ? -1 : mx_que.top();
    }
    
    void push_back(int value) {
        que.push(value);
        mx_que.push(value);
    }
    
    int pop_front() {
        if (que.empty()) {
            return -1;
        }
        int x = que.front(); que.pop();
        mx_que.pop(x);
        return x;
    }
private:
    queue<int> que;
    Queue mx_que;
};

/**
 * Your MaxQueue object will be instantiated and called as such:
 * MaxQueue* obj = new MaxQueue();
 * int param_1 = obj->max_value();
 * obj->push_back(value);
 * int param_3 = obj->pop_front();
 */
```