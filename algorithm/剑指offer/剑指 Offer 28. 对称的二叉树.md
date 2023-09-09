## [剑指 Offer 28. 对称的二叉树](https://leetcode.cn/problems/dui-cheng-de-er-cha-shu-lcof/)

请实现一个函数，用来判断一棵二叉树是不是对称的。如果一棵二叉树和它的镜像一样，那么它是对称的。

例如，二叉树 [1,2,2,3,4,4,3] 是对称的。

```
    `1      
    / \     
    2   2    
    / \ / \   
    3  4 4  3` 
``` 
但是下面这个 [1,2,2,null,3,null,3] 则不是镜像对称的:

```
`1      
/ \     
2   2      
\   \      
3    3`
```

**示例 1：**

**输入：**root = [1,2,2,3,4,4,3]
**输出：**true

**示例 2：**

**输入：**root = [1,2,2,null,3,null,3]
**输出：**false

**限制：**

`0 <= 节点个数 <= 1000`

### 解题思路

判断是否为镜像的时候，可以将根节点和根节点自己进行比较，递归比较一个节点的左节点是否和右节点是否相同，以及当前节点的值是否相同

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
    bool isSymmetric(TreeNode* root) {
        return helper(root, root);
    }

    bool helper(TreeNode* p, TreeNode* q) {
        if (!p && !q) {
            return true;
        }
        if (!p || !q) {
            return false;
        }
        return p->val == q->val && helper(p->left, q->right) && helper(p->right, q->left);
    }
};
```

可以将上面的递归改为迭代，引入一个队列，根节点加入两次，每次取出两个节点进行判断，然后插入子节点的时候，分别插入一个左节点一个右节点

```cpp
class Solution {
public:
    bool isSymmetric(TreeNode* root) {
        return helper(root, root);
    }

    bool helper(TreeNode* p, TreeNode* q) {
        queue<TreeNode*> que;
        que.push(p); que.push(q);
        while (!que.empty()) {
            p = que.front(); que.pop();
            q = que.front(); que.pop();
            if (!p && !q) continue;
            if (!p || !q) return false;
            if (p->val != q->val) return false;

            que.push(p->left);
            que.push(q->right);

            que.push(p->right);
            que.push(q->left);
        }
        return true;
    }
};
```