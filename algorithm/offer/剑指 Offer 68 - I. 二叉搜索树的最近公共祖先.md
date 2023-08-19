## [剑指 Offer 68 - I. 二叉搜索树的最近公共祖先](https://leetcode.cn/problems/er-cha-sou-suo-shu-de-zui-jin-gong-gong-zu-xian-lcof/)

给定一个二叉搜索树, 找到该树中两个指定节点的最近公共祖先。

[百度百科](https://baike.baidu.com/item/%E6%9C%80%E8%BF%91%E5%85%AC%E5%85%B1%E7%A5%96%E5%85%88/8918834?fr=aladdin) 中最近公共祖先的定义为：“对于有根树 T 的两个结点 p、q，最近公共祖先表示为一个结点 x，满足 x 是 p、q 的祖先且 x 的深度尽可能大（**一个节点也可以是它自己的祖先**）。”

例如，给定如下二叉搜索树:  root = [6,2,8,0,4,7,9,null,null,3,5]

![](https://assets.leetcode-cn.com/aliyun-lc-upload/uploads/2018/12/14/binarysearchtree_improved.png)

**示例 1:**

**输入:** root = [6,2,8,0,4,7,9,null,null,3,5], p = 2, q = 8  
**输出:** 6  
**解释:** 节点 `2` 和节点 `8` 的最近公共祖先是 `6。`

**示例 2:**

**输入:** root = [6,2,8,0,4,7,9,null,null,3,5], p = 2, q = 4  
**输出:** 2  
**解释:** 节点 `2` 和节点 `4` 的最近公共祖先是 `2`, 因为根据定义最近公共祖先节点可以为节点本身。

**说明:**

- 所有节点的值都是唯一的。
- p、q 为不同节点且均存在于给定的二叉搜索树中。

## 解题思路

题目中是二叉搜索树，可以根据性质：当前节点的左孩子一定都小于当前节点值，右孩子一定都大于当前节点值。这样的话，可以判断 p, q 是否在同一侧，在同一侧递归计算即可

```cpp
class Solution {
public:
    TreeNode* lowestCommonAncestor(TreeNode* root, TreeNode* p, TreeNode* q) {
        if (p->val < root->val && q->val < root->val) { // p,q 在左侧
            return lowestCommonAncestor(root->left, p, q);
        } else if (p->val > root->val && q->val > root->val) { // p,q 在右侧
            return lowestCommonAncestor(root->right, p, q);
        } 
        // 递归基
        return root;
    }
};
```