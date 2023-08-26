## [剑指 Offer 33. 二叉搜索树的后序遍历序列](https://leetcode.cn/problems/er-cha-sou-suo-shu-de-hou-xu-bian-li-xu-lie-lcof/)

输入一个整数数组，判断该数组是不是某二叉搜索树的后序遍历结果。如果是则返回 `true`，否则返回 `false`。假设输入的数组的任意两个数字都互不相同。

参考以下这颗二叉搜索树：

```
     5
    / \
   2   6  
  / \  
 1   3
```

**示例 1：**

**输入:** [1,6,3,2,5]  
**输出:** false

**示例 2：**

**输入:** [1,3,2,6,5]  
**输出:** true

**提示：** 1. `数组长度 <= 1000`

## 解题思路

二叉搜索树的性质是：中序遍历是递增的，所以将后序遍历排序一下即可得到中序遍历。然后利用中序遍历是【左，根，右】，后序遍历是【左，右，根】的顺序可以递归的判断每一颗子树是否满足要求。这里的要求是二叉搜索树的另外一个性质：对于任意的根节点，它的所有左孩子都不大于根节点，它的所有右孩子都不小于根节点；

```cpp
class Solution {
public:
    bool verifyPostorder(vector<int>& postorder) {
        auto inorder = postorder;
        sort(inorder.begin(), inorder.end());
        return check(inorder, postorder, 0, inorder.size() - 1, 0, postorder.size() - 1);
    }

    bool check(vector<int> &inorder, vector<int> &postorder, int in_left, int in_right, int post_left, int post_right) {
        if ((in_left > in_right) || (post_left > post_right)) {
            return true;
        }
        int root = postorder[post_right];
        int in_root = in_left;
        // 在中序遍历中找到根节点，由此可以计算出左子树的个数
        for (int i = in_left; i <= in_right; ++i) {
            if (inorder[i] == root) {
                in_root = i;
                break;
            }
        }
        int left_cnt = in_root - in_left;
        // 检查左子树，是否小于root
        for (int i = post_left; i < post_left + left_cnt; ++i) {
            if (postorder[i] > root) {
                return false;
            }
        }
        // 检查右子树，是否大于root
        for (int i = post_left + left_cnt; i < post_right; ++i) {
            if (postorder[i] < root) {
                return false;
            }
        }
        // 递归判断左右子树是否都满足条件
        return check(inorder, postorder, in_left, in_root - 1, post_left, post_left + left_cnt - 1) && check(inorder, postorder, in_root + 1, in_right, post_left + left_cnt, post_right - 1);
    }
};
```