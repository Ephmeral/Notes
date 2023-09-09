## [剑指 Offer 37. 序列化二叉树](https://leetcode.cn/problems/xu-lie-hua-er-cha-shu-lcof/)

请实现两个函数，分别用来序列化和反序列化二叉树。

你需要设计一个算法来实现二叉树的序列化与反序列化。这里不限定你的序列 / 反序列化算法执行逻辑，你只需要保证一个二叉树可以被序列化为一个字符串并且将这个字符串反序列化为原始的树结构。

**提示：**输入输出格式与 LeetCode 目前使用的方式一致，详情请参阅 [LeetCode 序列化二叉树的格式](https://support.leetcode-cn.com/hc/kb/article/1567641/)。你并非必须采取这种方式，你也可以采用其他的方法解决这个问题。

**示例：**

![](https://assets.leetcode.com/uploads/2020/09/15/serdeser.jpg)

**输入：**root = [1,2,3,null,null,4,5]  
**输出：**[1,2,3,null,null,4,5]

## 解题思路

序列化的时候利用层序遍历，将每层节点输出，以空格为分隔符，# 作为空节点；

反序列化的时候，从字符串中读取每个节点放入数组中，然后根据位置关系建立连接；

```cpp
class Codec {
public:
    // Encodes a tree to a single string.
    string serialize(TreeNode* root) {
        if (root == nullptr) return "";
        ostringstream output;
        queue<TreeNode*>que;
        que.push(root);
        while (!que.empty()) {
            TreeNode *x = que.front(); que.pop();
            if (x == nullptr) output << "# ";
            else {
                output << x->val << " ";
                que.push(x->left);
                que.push(x->right);
            }
        }
        return output.str();
    }

    // Decodes your encoded data to tree.
    TreeNode* deserialize(string data) {
        if (data.empty()) return nullptr;
        vector<TreeNode*> nodes;
        string val;
        istringstream input(data);
        while (input >> val) {
            if (val == "#") nodes.push_back(nullptr);
            else nodes.push_back(new TreeNode(stoi(val)));
        }
        int pos = 1;
        for (int i = 0; i < nodes.size(); ++i) {
            if (nodes[i] == nullptr) continue;
            nodes[i]->left = nodes[pos++];
            nodes[i]->right = nodes[pos++];
        }
        return nodes[0];
    }
};
```