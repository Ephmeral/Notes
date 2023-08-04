## [剑指 Offer 34. 二叉树中和为某一值的路径](https://leetcode.cn/problems/er-cha-shu-zhong-he-wei-mou-yi-zhi-de-lu-jing-lcof/)

给你二叉树的根节点 `root` 和一个整数目标和 `targetSum` ，找出所有 **从根节点到叶子节点** 路径总和等于给定目标和的路径。

**叶子节点** 是指没有子节点的节点。

**示例 1：**

![](https://assets.leetcode.com/uploads/2021/01/18/pathsumii1.jpg)

**输入：**root = [5,4,8,11,null,13,4,7,2,null,null,5,1], targetSum = 22  
**输出：**[[5,4,11,2],[5,8,4,5]]

**示例 2：**

![](https://assets.leetcode.com/uploads/2021/01/18/pathsum2.jpg)

**输入：**root = [1,2,3], targetSum = 5  
**输出：**[]

**示例 3：**

**输入：**root = [1,2], targetSum = 0  
**输出：**[]

**提示：**

- 树中节点总数在范围 `[0, 5000]` 内
- `-1000 <= Node.val <= 1000`
- `-1000 <= targetSum <= 1000`

## 解题思路

回溯法，递归终点为 root == nullptr 的时候，当 root 为叶子节点的时候，判断 root 的值是否等于 target，此为一条合法的路径；

```cpp
/**
 * Definition for a binary tree node.
 * struct TreeNode {
 *     int val;
 *     TreeNode *left;
 *     TreeNode *right;
 *     TreeNode() : val(0), left(nullptr), right(nullptr) {}
 *     TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
 *     TreeNode(int x, TreeNode *left, TreeNode *right) : val(x), left(left), right(right) {}
 * };
 */
class Solution {
    vector<int> path;
    vector<vector<int>> res;
public:
    vector<vector<int>> pathSum(TreeNode* root, int target) {
        dfs(root, target);
        return res;
    }

    void dfs(TreeNode* root, int target) {
        if (root == nullptr) {
            return;
        }
        path.emplace_back(root->val);
        if (root->left == nullptr && root->right == nullptr && target == root->val) {
            res.emplace_back(path);
        }
        dfs(root->left, target - root->val);
        dfs(root->right, target - root->val);
        path.pop_back();
    }
};
```