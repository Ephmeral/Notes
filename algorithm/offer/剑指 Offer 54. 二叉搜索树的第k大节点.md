## [剑指 Offer 54. 二叉搜索树的第k大节点](https://leetcode.cn/problems/er-cha-sou-suo-shu-de-di-kda-jie-dian-lcof/)


给定一棵二叉搜索树，请找出其中第 `k` 大的节点的值。

**示例 1:**

**输入:** root = [3,1,4,null,2], k = 1  
   3  
  / \  
 1 4  
  \  
   2  
**输出:** 4

**示例 2:**

**输入:** root = [5,3,6,2,4,null,null,1], k = 3  
       5  
      / \  
     3 6  
    / \  
   2 4  
  /  
 1  
**输出:** 4

**限制：**
- 1 ≤ k ≤ 二叉搜索树元素个数

## 解题思路

二叉搜索树的中序遍历是递增的，如果将中序遍历倒过来则是递减的，依据此来找到第 k 大的数

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
    int kthLargest(TreeNode* root, int k) {
        dfs(root, k);
        return res;
    }

    int res = 0;
    int cnt = 0;

    void dfs(TreeNode* root, int k) {
        if (root == nullptr) {
            return;
        }
        dfs(root->right, k);
        if (++cnt == k) {
            res = root->val;
        }
        dfs(root->left, k);
    }
};
```