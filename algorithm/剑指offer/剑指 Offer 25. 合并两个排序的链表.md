## [剑指 Offer 25. 合并两个排序的链表](https://leetcode.cn/problems/he-bing-liang-ge-pai-xu-de-lian-biao-lcof/)

输入两个递增排序的链表，合并这两个链表并使新链表中的节点仍然是递增排序的。

**示例 1：**

**输入：**1->2->4, 1->3->4  
**输出：**1->1->2->3->4->4

**限制：**

`0 <= 链表长度 <= 1000`

### 解题思路

1. 递归法：
- 递归终点是两个节点有一个为空的情况，直接返回另外一个节点；
- 否则根据两个节点大小进行判断，并将值大的与小的 next 指针再进行合并；
- 最后返回小的节点；

```cpp
class Solution {
public:
    ListNode* mergeTwoLists(ListNode* p, ListNode* q) {
        if (p == nullptr) {
            return q;
        }
        if (q == nullptr) {
            return p;
        }
        if (p->val < q->val) {
            p->next = mergeTwoLists(p->next, q);
            return p;
        } 
        q->next = mergeTwoLists(p, q->next);
        return q;
    }
};
```

2. 迭代法
- 引入一个头结点，便于操作；
- 遍历链表 p 和 q，循环条件是 p 和 q 都不为 nullptr，每次将值小的插入到新链表上；
- 最后再单独判断 p 和 q 是否不为空，不为空的插入到新链表的结尾；

```cpp
class Solution {
public:
    ListNode* mergeTwoLists(ListNode* p, ListNode* q) {
        ListNode *head = new ListNode(0);
        ListNode *cur = head;
        while (p != nullptr && q != nullptr) {
            if (p->val < q->val) {
                cur->next = p;
                p = p->next;
            } else {
                cur->next = q;
                q = q->next;
            }
            cur = cur->next;
        }
        if (p != nullptr) {
            cur->next = p;
        }
        if (q != nullptr) {
            cur->next = q;
        }
        ListNode *res = head->next;
        delete head;
        return res;
    }
};
```