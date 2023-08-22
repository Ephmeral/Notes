## [剑指 Offer 07. 重建二叉树](https://leetcode.cn/problems/zhong-jian-er-cha-shu-lcof/)

输入某二叉树的前序遍历和中序遍历的结果，请构建该二叉树并返回其根节点。

假设输入的前序遍历和中序遍历的结果中都不含重复的数字。

**示例 1:**

![](https://assets.leetcode.com/uploads/2021/02/19/tree.jpg)

**Input:** preorder = [3,9,20,15,7], inorder = [9,3,15,20,7]  
**Output:** [3,9,20,null,null,15,7]

**示例 2:**

**Input:** preorder = [-1], inorder = [-1]  
**Output:** [-1]

**限制：**

`0 <= 节点个数 <= 5000`

## 解题思路

前序遍历是 【根，左，右】，中序遍历是 【左，根，右】，以此可以知道前序遍历的第一个为根节点，然后在中序遍历中找到根节点对应位置，即可计算出左孩子和右孩子的节点个数，然后分治的递归求解左子树和右子树即可：

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
    TreeNode* buildTree(vector<int>& preorder, vector<int>& inorder) {
        return build(preorder, inorder, 0, preorder.size() - 1, 0, inorder.size() - 1);
    }

    TreeNode* build(vector<int>& preorder, vector<int> &inorder, int pre_left, int pre_right, int in_left, int in_right) {
        if (pre_left > pre_right) return nullptr;
        if (in_left > in_right) return nullptr;
        int root = preorder[pre_left];
        int in_root = in_left;
        for (int i = in_left; i <= in_right; ++i) {
            if (inorder[i] == root) {
                in_root = i;
                break;
            }
        }
        int left_cnt = in_root - in_left;
        int right_cnt = in_right - in_root;
        TreeNode *tree = new TreeNode(root);
        tree->left = build(preorder, inorder, pre_left + 1, pre_left + left_cnt, in_left, in_root - 1);
        tree->right = build(preorder, inorder, pre_left + left_cnt + 1, pre_right, in_root + 1, in_right);
        return tree;
    }
};
```