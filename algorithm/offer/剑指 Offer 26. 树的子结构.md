## [剑指 Offer 26. 树的子结构](https://leetcode.cn/problems/shu-de-zi-jie-gou-lcof/)

输入两棵二叉树 A 和 B，判断 B 是不是 A 的子结构。(约定空树不是任意一个树的子结构)

B 是 A 的子结构， 即 A 中有出现和 B 相同的结构和节点值。

例如:  
给定的树 A:

```
     3       
   / \     
  4   5     
 / \    
1   2  
```
给定的树 B：

```
   4      
  /    
  1  
```
返回 true，因为 B 与 A 的一个子树拥有相同的结构和节点值。

**示例 1：**

**输入：**A = [1,2,3], B = [3,1]  
**输出：**false

**示例 2：**

**输入：**A = [3,4,5,1,2], B = [4,1]  
**输出：**true

**限制：**

`0 <= 节点个数 <= 10000`

### 解题思路

递归判断所有的 A 节点，是否包含 B 的结构，需要单独开个函数进行检查，检查的时候：
- 如果 B == nullptr 说明 B 已经检查完了，返回 true；
- 如果 A == nullptr 说明此时 A 不能包含所有的 B 节点；
- 如果 A->val != B->val 返回false；
- 然后递归检查每个对应的子节点；

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
    bool isSubStructure(TreeNode* A, TreeNode* B) {
        if (A == nullptr || B == nullptr) return false;
        return check(A, B) || isSubStructure(A->left, B) || isSubStructure(A->right, B);
    }

    bool check(TreeNode* A, TreeNode* B) {
        if (B == nullptr) {
            return true;
        }
        if (A == nullptr) {
            return false;
        }
        if (A->val != B->val) {
            return false;
        }
        return (check(A->left, B->left) && check(A->right, B->right));
    }
};
```