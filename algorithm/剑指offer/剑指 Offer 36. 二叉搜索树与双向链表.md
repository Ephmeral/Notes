## [剑指 Offer 36. 二叉搜索树与双向链表](https://leetcode.cn/problems/er-cha-sou-suo-shu-yu-shuang-xiang-lian-biao-lcof/)

输入一棵二叉搜索树，将该二叉搜索树转换成一个排序的循环双向链表。要求不能创建任何新的节点，只能调整树中节点指针的指向。

为了让您更好地理解问题，以下面的二叉搜索树为例：

![](https://assets.leetcode.com/uploads/2018/10/12/bstdlloriginalbst.png)

我们希望将这个二叉搜索树转化为双向循环链表。链表中的每个节点都有一个前驱和后继指针。对于双向循环链表，第一个节点的前驱是最后一个节点，最后一个节点的后继是第一个节点。

下图展示了上面的二叉搜索树转化成的链表。“head” 表示指向链表中有最小元素的节点。

![](https://assets.leetcode.com/uploads/2018/10/12/bstdllreturndll.png)

特别地，我们希望可以就地完成转换操作。当转化完成以后，树中节点的左指针需要指向前驱，树中节点的右指针需要指向后继。还需要返回链表中的第一个节点的指针。


## 解题思路

二叉搜索树中序遍历得到的结果是按照顺序排序的，所以这题考虑用中序遍历，遍历的时候需要一个 head 指针指向头结点，pre 指针指向当前节点遍历的上一个节点，这样就可以将当前节点指向上一个节点，代码如下：

```cpp
/*
// Definition for a Node.
class Node {
public:
    int val;
    Node* left;
    Node* right;

    Node() {}

    Node(int _val) {
        val = _val;
        left = NULL;
        right = NULL;
    }

    Node(int _val, Node* _left, Node* _right) {
        val = _val;
        left = _left;
        right = _right;
    }
};
*/
class Solution {
public:
    Node* treeToDoublyList(Node* root) {
        if (root == nullptr) {
            return root;
        }
        dfs(root);
        head->left = pre; // 首尾相连
        pre->right = head;
        return head;
    }

private:
    Node *pre, *head;
    
    void dfs(Node* cur) {
        if (cur == nullptr) {
            return;
        }
        dfs(cur->left);
        if (pre != nullptr) pre->right = cur; // 连接上一个节点
        else head = cur; // 记录头结点
        cur->left = pre;
        pre = cur;
        dfs(cur->right);
    }
};
```