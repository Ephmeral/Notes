## [剑指 Offer 27. 二叉树的镜像](https://leetcode.cn/problems/er-cha-shu-de-jing-xiang-lcof/)

请完成一个函数，输入一个二叉树，该函数输出它的镜像。

例如输入：

```
     `4      
     /   \     
     2     7    
    / \   / \   
   1   3 6   9`
```  
镜像输出：

 ```
`4      
/   \     
7     2    
/ \   / \   
9   6 3   1`
```

**示例 1：**

**输入：**root = [4,2,7,1,3,6,9]  
**输出：**[4,7,2,9,6,3,1]

**限制：**

`0 <= 节点个数 <= 1000`

### 解题思路

反转的时候递归将每个子节点反转即可

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
    TreeNode* mirrorTree(TreeNode* root) {
        if (root == nullptr) {
            return root;
        }
        TreeNode* right = mirrorTree(root->left);
        TreeNode* left = mirrorTree(root->right);
        root->left = left;
        root->right = right;
        return root;
    }
};
```