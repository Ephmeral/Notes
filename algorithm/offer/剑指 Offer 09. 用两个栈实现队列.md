## [剑指 Offer 09. 用两个栈实现队列](https://leetcode.cn/problems/yong-liang-ge-zhan-shi-xian-dui-lie-lcof/)

用两个栈实现一个队列。队列的声明如下，请实现它的两个函数 `appendTail` 和 `deleteHead` ，分别完成在队列尾部插入整数和在队列头部删除整数的功能。(若队列中没有元素，`deleteHead` 操作返回 -1 )

**示例 1：**

**输入：**  
["CQueue","appendTail","deleteHead","deleteHead","deleteHead"]  
[[],[3],[],[],[]]  
**输出：**[null,null,3,-1,-1]

**示例 2：**

**输入：**  
["CQueue","deleteHead","appendTail","appendTail","deleteHead","deleteHead"]  
[[],[],[5],[2],[],[]]  
**输出：**[null,-1,null,null,5,2]

**提示：**

- `1 <= values <= 10000`
- 最多会对 `appendTail、deleteHead` 进行 `10000` 次调用

### 解题思路

基本思路就是一个栈 st1 存放数据，当 deleteHead() 的时候优先从另外一个栈 st2 取数据，如果 st2 为空，则将 st1 的数据都移入到 st2 中，刚好实现了先入先出的队列形式；

```cpp
class CQueue {
public:
    CQueue() {

    }
    
    void appendTail(int value) {
        st1.emplace(value);
    }
    
    int deleteHead() {
        if (st2.empty()) {
            while (!st1.empty()) {
                st2.emplace(st1.top());
                st1.pop();
            }
        }
        if (st2.empty()) {
            return -1;
        }
        int res = st2.top();
        st2.pop();
        return res;
    }

private:
    stack<int> st1;
    stack<int> st2;
};

/**
 * Your CQueue object will be instantiated and called as such:
 * CQueue* obj = new CQueue();
 * obj->appendTail(value);
 * int param_2 = obj->deleteHead();
 */
```