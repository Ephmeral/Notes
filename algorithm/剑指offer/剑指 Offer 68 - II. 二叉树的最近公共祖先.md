## [剑指 Offer 68 - II. 二叉树的最近公共祖先](https://leetcode.cn/problems/er-cha-shu-de-zui-jin-gong-gong-zu-xian-lcof/)

给定一个二叉树, 找到该树中两个指定节点的最近公共祖先。

[百度百科](https://baike.baidu.com/item/%E6%9C%80%E8%BF%91%E5%85%AC%E5%85%B1%E7%A5%96%E5%85%88/8918834?fr=aladdin) 中最近公共祖先的定义为：“对于有根树 T 的两个结点 p、q，最近公共祖先表示为一个结点 x，满足 x 是 p、q 的祖先且 x 的深度尽可能大（**一个节点也可以是它自己的祖先**）。”

例如，给定如下二叉树:  root = [3,5,1,6,2,0,8,null,null,7,4]

![](https://assets.leetcode-cn.com/aliyun-lc-upload/uploads/2018/12/15/binarytree.png)

**示例 1:**

**输入:** root = [3,5,1,6,2,0,8,null,null,7,4], p = 5, q = 1  
**输出:** 3  
**解释:** 节点 `5` 和节点 `1` 的最近公共祖先是节点 `3。`

**示例 2:**

**输入:** root = [3,5,1,6,2,0,8,null,null,7,4], p = 5, q = 4  
**输出:** 5  
**解释:** 节点 `5` 和节点 `4` 的最近公共祖先是节点 `5。` 因为根据定义最近公共祖先节点可以为节点本身。

**说明:**

- 所有节点的值都是唯一的。
- p、q 为不同节点且均存在于给定的二叉树中。

## 解题思路

`lowestCommonAncestor` 函数返回值定义为节点 p 和 q 的公共祖先，但是我们可以多给它一层含义：

1. 若树里面存在 p，也存在 q，则返回他们的公共祖先。
2. 若树里面只存在 p，或只存在 q，则返回存在的那一个。
3. 若树里面即不存在 p，也不存在 q，则返回 null。

递归的时候，后序遍历思路，分别得到左右子树的结果，如果 l, r 任一为空，说明答案在另外一棵子树或祖先中；如果 l, r 都不空，说明 p, q 位于 root 两侧，返回 root

```cpp
class Solution {
public:
    TreeNode* lowestCommonAncestor(TreeNode* root, TreeNode* p, TreeNode* q) {
        if (root == nullptr || p == root || q == root) {
            return root;
        }
        // l、r 非空时，说明 p、q 分居 root 的两侧，root 就是 LCA
        // l、r 任一为空，说明 LCA 位于另一子树或其祖先中
        TreeNode* l = lowestCommonAncestor(root->left, p, q);
        TreeNode* r = lowestCommonAncestor(root->right, p, q);
        return l == nullptr ? r : (r == nullptr ? l : root);
    }
};
```