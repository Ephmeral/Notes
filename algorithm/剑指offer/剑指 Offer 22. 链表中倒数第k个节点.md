## [剑指 Offer 22. 链表中倒数第k个节点](https://leetcode.cn/problems/lian-biao-zhong-dao-shu-di-kge-jie-dian-lcof/)

输入一个链表，输出该链表中倒数第 k 个节点。为了符合大多数人的习惯，本题从 1 开始计数，即链表的尾节点是倒数第 1 个节点。

例如，一个链表有 `6` 个节点，从头节点开始，它们的值依次是 `1、2、3、4、5、6`。这个链表的倒数第 `3` 个节点是值为 `4` 的节点。

**示例：**

给定一个链表: **1->2->3->4->5**, 和 _k_ **= 2**.

返回链表 **4->5**.

### 解题思路

先引入头结点，然后利用双指针，快指针依次遍历，当已经遍历 k 个节点的时候，慢指针也跟着移动，当快指针到达 nullptr 的时候，慢指针指向的位置就是结果。

```cpp
class Solution {
public:
    ListNode* getKthFromEnd(ListNode* head, int k) {
        ListNode* dummy = new ListNode(0);
        dummy->next = head;
        ListNode *pre = dummy, *cur = dummy;
        int cnt = 0;
        while (cur != nullptr) {
            if (cnt >= k) {
                pre = pre->next;
            }
            cur = cur->next;
            cnt++;
        }
        delete dummy;
        return pre;
    }
};
```