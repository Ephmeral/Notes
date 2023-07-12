# [剑指 Offer 06. 从尾到头打印链表](https://leetcode.cn/problems/cong-wei-dao-tou-da-yin-lian-biao-lcof/)

输入一个链表的头节点，从尾到头反过来返回每个节点的值（用数组返回）。

**示例 1：**

输入：head = [1,3,2]  
输出：[2,3,1]

**限制：**`0 <= 链表长度 <= 10000`

## 解题思路
### 1. 暴力
最暴力直接方法就是遍历一遍链表，然后再反转：

```cpp
/**
 * Definition for singly-linked list.
 * struct ListNode {
 *     int val;
 *     ListNode *next;
 *     ListNode(int x) : val(x), next(NULL) {}
 * };
 */
class Solution {
public:
    vector<int> reversePrint(ListNode* head) {
        vector<int> res;
        while (head != nullptr) {
            res.push_back(head->val);
            head = head->next;
        }
        reverse(res.begin(), res.end());
        return res;
    }
};
```

### 2. 递归

第二种思路是利用递归，类似于二叉树的后序遍历，递归终点是 head == nullptr。

```cpp
class Solution {
public:
    vector<int> reversePrint(ListNode* head) {
        vector<int> res;
        dfs(head, res);
        return res;
    }

    void dfs(ListNode *head, vector<int> &res) {
        // 递归终点
        if (head == nullptr) {
            return ;
        }
        // 递归下一个节点
        dfs(head->next, res);
        // 将当前节点值放入res中
        res.push_back(head->val);
    }
};
```

### 3. 栈
第三种方法就是利用栈，和递归思路差不多，因为递归本质就是利用栈的。

```cpp
class Solution {
public:
    vector<int> reversePrint(ListNode* head) {
        stack<int> st;
        // 遍历链表，将数据放入栈中
        while (head != nullptr) {
            st.emplace(head->val);
            head = head->next;
        }
        vector<int> res;
        // 从栈中依次取出元素，放入res当中
        while (!st.empty()) {
            res.push_back(st.top());
            st.pop();
        }
        return res;
    }
};
```