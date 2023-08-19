## [剑指 Offer 55 - II. 平衡二叉树](https://leetcode.cn/problems/ping-heng-er-cha-shu-lcof/)

输入一棵二叉树的根节点，判断该树是不是平衡二叉树。如果某二叉树中任意节点的左右子树的深度相差不超过 1，那么它就是一棵平衡二叉树。

**示例 1:**

给定二叉树 `[3,9,20,null,null,15,7]`

```
    3
   / \
  9  20
    /  \
   15   7
```

返回 `true` 。  
  
**示例 2:**

给定二叉树 `[1,2,2,3,3,null,null,4,4]`

```
       1
      / \
     2   2
    / \
   3   3
  / \
 4   4
```

返回 `false` 。

**限制：**

- `0 <= 树的结点个数 <= 10000`

## 解题思路

采用后序遍历思路，递归计算高度，计算高度的同时判断每个节点是否平衡

```cpp
/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode(int x) : val(x), left(NULL), right(NULL) {}
 * };
 */
class Solution {
public:
    bool isBalanced(TreeNode* root) {
        if (root == nullptr) {
            return true;
        }
        getHeight(root);
        return res;
    }
    bool res = true;
    int getHeight(TreeNode* root) {
        if (root == nullptr) {
            return 0;
        }
        int left = getHeight(root->left);
        int right = getHeight(root->right);
        if (abs(left - right) > 1) {
            res = false;
        }
        return 1 + max(left, right);
    }
};
```