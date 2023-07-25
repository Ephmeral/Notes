这是有三道题的，但实际上差别不大，会层序遍历就大概能知道如何解决，大体上都是一个模版

## [剑指 Offer 32 - I. 从上到下打印二叉树](https://leetcode.cn/problems/cong-shang-dao-xia-da-yin-er-cha-shu-lcof/)

从上到下打印出二叉树的每个节点，同一层的节点按照从左到右的顺序打印。

例如:  给定二叉树: `[3,9,20,null,null,15,7]`,

```
    3
   / \
  9  20
    /  \
   15   7
```

返回：

```
[3,9,20,15,7]
```

**提示：** 1. `节点总数 <= 1000`

### 解题思路

这是最简单的一种，直接层序遍历一下，我这里代码稍微复杂了一点，因为可以适配后面两题

```cpp
class Solution {
public:
    vector<int> levelOrder(TreeNode* root) {
        if (root == nullptr) {
            return {};
        }
        vector<int> res;
        queue<TreeNode*> que;
        que.push(root);
        while (!que.empty()) {
            int sz = que.size();
            // 这里不用for循环也没问题
            for (int i = 0; i < sz; ++i) {
                TreeNode *p = que.front(); que.pop();
                if (p->left) {
                    que.push(p->left);
                }
                if (p->right) {
                    que.push(p->right);
                }
                res.push_back(p->val);
            }
        }
        return res;
    }
};
```

## [剑指 Offer 32 - II. 从上到下打印二叉树 II](https://leetcode.cn/problems/cong-shang-dao-xia-da-yin-er-cha-shu-ii-lcof/)

从上到下按层打印二叉树，同一层的节点按从左到右的顺序打印，每一层打印到一行。

例如:  给定二叉树: `[3,9,20,null,null,15,7]`,

```
    3
   / \
  9  20
    /  \
   15   7
```

返回其层次遍历结果：

```
[
  [3],
  [9,20],
  [15,7]
]
```

**提示：** 1. `节点总数 <= 1000`

### 解题思路

比上个题目来说，需要将每层区分出来，上面的模版就能用上了

```cpp
class Solution {
public:
    vector<vector<int>> levelOrder(TreeNode* root) {
        if (root == nullptr) {
            return {};
        }
        vector<vector<int>> res;
        queue<TreeNode*> que;
        que.push(root);
        while (!que.empty()) {
            int sz = que.size();
            vector<int> level; // 保存每层的元素
            for (int i = 0; i < sz; ++i) {
                TreeNode *p = que.front(); que.pop();
                if (p->left) {
                    que.push(p->left);
                }
                if (p->right) {
                    que.push(p->right);
                }
                level.push_back(p->val);
            }
            res.push_back(level);
        }
        return res;
    }
};
```

## [剑指 Offer 32 - III. 从上到下打印二叉树 III](https://leetcode.cn/problems/cong-shang-dao-xia-da-yin-er-cha-shu-iii-lcof/)

请实现一个函数按照之字形顺序打印二叉树，即第一行按照从左到右的顺序打印，第二层按照从右到左的顺序打印，第三行再按照从左到右的顺序打印，其他行以此类推。

例如:  
给定二叉树: `[3,9,20,null,null,15,7]`,

```
    3
   / \
  9  20
    /  \
   15   7
```

返回其层次遍历结果：

```
[
  [3],
  [20,9],
  [15,7]
]
```

**提示：** 1. `节点总数 <= 1000`

### 解题思路

这题和上题区别在于每隔一层输出的顺序为逆序的，可以继续套用上面的模版，然后每个一层将数组反转，再放入的结果中

```cpp
class Solution {
public:
    vector<vector<int>> levelOrder(TreeNode* root) {
        if (root == nullptr) {
            return {};
        }
        vector<vector<int>> res;
        queue<TreeNode*> que;
        que.push(root);
        int cnt = 0;
        while (!que.empty()) {
            int sz = que.size();
            vector<int> level;
            for (int i = 0; i < sz; ++i) {
                TreeNode *p = que.front(); que.pop();                
                if (p->left) que.push(p->left);
                if (p->right) que.push(p->right);
                level.push_back(p->val);
            }
            if (cnt & 1) reverse(level.begin(), level.end());
            res.push_back(level);
            ++cnt;
        }
        return res;
    }
};
```

另外一种方法是，利用双端队列，当下一层需要反转的时候，插入元素插入到队列前面，这样即可实现反转

```cpp
class Solution {
public:
    vector<vector<int>> levelOrder(TreeNode* root) {
        if (root == nullptr) {
            return {};
        }
        vector<vector<int>> res;
        queue<TreeNode*> que;
        que.push(root);
        int cnt = 0;
        while (!que.empty()) {
            int sz = que.size();
            deque<int> level;
            for (int i = 0; i < sz; ++i) {
                TreeNode *p = que.front(); que.pop();
                if (p->left) que.push(p->left);
                if (p->right) que.push(p->right);
                if (cnt & 1) {
                    level.push_front(p->val);
                } else {
                    level.push_back(p->val);
                }
            }
            // if (cnt & 1) reverse(level.begin(), level.end());
            res.push_back(vector<int>{level.begin(), level.end()});
            ++cnt;
        }
        return res;
    }
};
```