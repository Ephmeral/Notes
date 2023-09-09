## [剑指 Offer 30. 包含min函数的栈](https://leetcode.cn/problems/bao-han-minhan-shu-de-zhan-lcof/)

定义栈的数据结构，请在该类型中实现一个能够得到栈的最小元素的 min 函数在该栈中，调用 min、push 及 pop 的时间复杂度都是 O(1)。

**示例:**

MinStack minStack = new MinStack();  
minStack.push(-2);  
minStack.push(0);  
minStack.push(-3);  
minStack.min(); --> 返回 -3.  
minStack.pop();  
minStack.top(); --> 返回 0.  
minStack.min(); --> 返回 -2.

**提示：**

1. 各函数的调用总次数不超过 20000 次

### 解题思路

1. 辅助栈

用一个辅助栈保存最小值，每次加入数据进来的时候，比较辅助栈和加入的 x 大小，将最小值放入辅助栈中。这样辅助栈的顶部将一直是最小值。

```cpp
    class MinStack {
    public:
        /** initialize your data structure here. */
        MinStack() {
            min_st.emplace(INT_MAX);
        }
        
        void push(int x) {
            st.emplace(x);
            min_st.emplace(::min(min_st.top(), x));
        }
        
        void pop() {
            st.pop();
            min_st.pop();
        }
        
        int top() {
            return st.top();
        }
        
        int min() {
            return min_st.top();
        }
    private:
        stack<int> st;
        stack<int> min_st;
    };

    /**
    * Your MinStack object will be instantiated and called as such:
    * MinStack* obj = new MinStack();
    * obj->push(x);
    * obj->pop();
    * int param_3 = obj->top();
    * int param_4 = obj->min();
    */
```

2. 差值存储

差值存储的思路是维护一个最小值 mi，插入栈中数据为 x - mi，当需要 pop 的时候或者 top 的时候需要更新对应的值。

- push
	- 插入 x 的时候，实际在栈中存放的是 x - mi；
	- 然后根据需要更新最小值 mi；
- pop
	- 如果顶部数据为负数，说明之前更新过最小值，此时需要还原之前的最小值；
	- 假设之前的最小值为 pre_min，当前最小值为 cur_min；
	- st.top() = x - pre_min；
	- 而 cur_min = x（因为后面更新了最小值）；
       - 所以 cur_min - st.top() = pre_min；
- top
	- 如果栈顶为负数，此时 top 数据 x 即为 mi（因为更新最小值的时候，mi = x）；
	- 如果栈顶为正数，需要更新栈顶数据（因为插入的是 x - mi），所以返回 st.top() + x；
- min
	- 返回当前的 mi 即可；

```cpp
class MinStack {
public:
    /** initialize your data structure here. */
    MinStack() {

    }
    
    void push(int x) {
        // 插入的数据通过差值插入，即 x - mi
        // 开始插入0，并更新mi = x
        if (st.empty()) {
            st.emplace(0);
            mi = x;
        } else {
            // 之后将差值 x - mi插入
            // 如果x大于mi，插入的数是正数，最小值不需要更新
            // 如果x小于mi，插入的数是负数，最小值需要更新
            st.emplace((long long)x - mi);
            mi = std::min(mi, (long long)x);
        }
    }
    
    void pop() {
        // 如果顶部数据为负数，说明之前更新过最小值
        // 此时需要还原之前的最小值
        // 假设之前的最小值为pre_min，当前最小值为cur_min
        // st.top() = x - pre_min
        // 而cur_min = x（因为后面更新了最小值）
        // 所以cur_min - st.top() = pre_min
        if (st.top() < 0) {
            mi -= st.top();
        }
        st.pop();
    }
    
    int top() {
        // 如果顶部数据为负数，说明当前最小值即为top
        // 否则需要还原顶部数据，因为st.top() = x - mi
        if (st.top() < 0) {
            return mi;
        } else {
            return st.top() + mi;
        }
    }
    
    int min() {
        return mi;
    }

private:
    stack<long long> st;
    long long mi; // 最小值
};
```