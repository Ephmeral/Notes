## [剑指 Offer 35. 复杂链表的复制](https://leetcode.cn/problems/fu-za-lian-biao-de-fu-zhi-lcof/)

请实现 `copyRandomList` 函数，复制一个复杂链表。在复杂链表中，每个节点除了有一个 `next` 指针指向下一个节点，还有一个 `random` 指针指向链表中的任意节点或者 `null`。

**示例 1：**

![](https://assets.leetcode-cn.com/aliyun-lc-upload/uploads/2020/01/09/e1.png)

**输入：**`head = [[7,null],[13,0],[11,4],[10,2],[1,0]]`  
**输出：**`[[7,null],[13,0],[11,4],[10,2],[1,0]]`

**示例 2：**

![](https://assets.leetcode-cn.com/aliyun-lc-upload/uploads/2020/01/09/e2.png)

**输入：**`head = [[1,1],[2,1]]  `  
**输出：**`[[1,1],[2,1]]`

**示例 3：**

**![](https://assets.leetcode-cn.com/aliyun-lc-upload/uploads/2020/01/09/e3.png)**

**输入：**`head = [[3,null],[3,0],[3,null]]  `  
**输出：**`[[3,null],[3,0],[3,null]]`

**示例 4：**

**输入：**head = []  
**输出：**[]  
**解释：**给定的链表为空（空指针），因此返回 null。

**提示：**

- `-10000 <= Node.val <= 10000`
- `Node.random` 为空（null）或指向链表中的节点。
- 节点数目不超过 1000 。

## 解题思路
### 1. 哈希表

下面是我的开始的写法，想太复杂了，或者是写的代码很不优雅

基本思路就是创建一个哈希表建立原节点到新节点的映射，每次拷贝的时候，判断是否已经创建了新的节点，没有的话新建立一个节点，有的话直接取出哈希表的节点作为 next 或者 random 指针指向的节点，代码如下：

```cpp
/*
// Definition for a Node.
class Node {
public:
    int val;
    Node* next;
    Node* random;
    
    Node(int _val) {
        val = _val;
        next = NULL;
        random = NULL;
    }
};
*/
// 下面这个可以不用写，开始不知道力扣给实现了
template<>
class hash<Node> {
public:
	size_t operator()(const Node& p)const {
		return hash<int>()(p.val)+ hash<int*>()((int*)p.next)+ hash<int*>()((int*)p.random);
	}
};

class Solution {
public:
    Node* copyRandomList(Node* head) {
        if (head == nullptr) {
            return nullptr;
        }
        Node *newHead = new Node(head->val);
        unordered_map<Node*, Node*> mp;
        mp[head] = newHead; // 建立头结点之间的映射
        Node *p = head, *np = newHead;
        while (p != nullptr) {
            // 1. 考虑复制next节点
            // 判断mp中是否存在np->next对应的节点（因为可能由之前的random新创建）
            if (mp.count(p->next)) {
                np->next = mp[p->next];
            } else {
                if (p->next != nullptr) {
                    np->next = new Node(p->next->val);
                    mp[p->next] = np->next;
                }
            }
            // 2. 复制random节点
            // 判断mp中是否存在np->random对应的节点（因为可能由之前的next新创建）
            if (mp.count(p->random)) {
                np->random = mp[p->random];
            } else {
                if (p->random != nullptr) {
                    np->random = new Node(p->random->val);
                    mp[p->random] = np->random;
                } 
            }
            np = np->next;
            p = p->next;
        }
        return newHead;
    }
};
```

简化写法：也是利用哈希表，先遍历一遍并创建新的节点，用哈希表在两者之间建立映射，之后再考虑通过将 next 和 random 指针连接起来

```cpp
class Solution {
public:
    Node* copyRandomList(Node* head) {
        if (head == nullptr) {
            return nullptr;
        }
        unordered_map<Node*, Node*> mp;
        Node *p = head;
        // 1. 创建所有节点的拷贝
        while (p != nullptr) {
            mp[p] = new Node(p->val);
            p = p->next;
        }
        Node *np = mp[head];
        p = head;
        // 2. 根据哈希表的节点建立连接
        while (p != nullptr) {
            np->next = mp[p->next];
            np->random = mp[p->random];
            np = np->next;
            p = p->next;
        }
        return mp[head];
    }
};
```

### 2. 递归法

递归的思路也是需要一个哈希表，copyRandomList 的语义是拷贝 head 链表，这样的话：“
1. 先判断哈希表中是否对应的映射，有直接返回；
2. 没有映射，说明需要创建新的节点，创建映射之后，在分别通过 copyRandomList 来拷贝 head->next 和 head->random；

```cpp
class Solution {
    unordered_map<Node*, Node*> mp;
public:
    Node* copyRandomList(Node* head) {
        if (head == nullptr) {
            return nullptr;
        }
        if (!mp.count(head)) {
            Node *newHead = new Node(head->val);
            mp[head] = newHead;
            newHead->next = copyRandomList(head->next);
            newHead->random = copyRandomList(head->random);
        } 
        return mp[head];
    }
};
```

### 3. 插入 + 拆分

考虑不用哈希表，复制各个节点，将其插入到原节点的下一个位置，这样的话 random 定位到原节点的下一个节点就是新的节点，所以再修改 random 的指向，最后将链表拆分，即可得到新的拷贝链表；

```cpp
class Solution {
public:
    Node* copyRandomList(Node* head) {
        if(head == nullptr) return nullptr;
        Node* cur = head;
        // 1. 复制各节点，并构建拼接链表
        while(cur != nullptr) {
            Node* tmp = new Node(cur->val);
            tmp->next = cur->next;
            cur->next = tmp;
            cur = tmp->next;
        }
        // 2. 构建各新节点的 random 指向
        cur = head;
        while(cur != nullptr) {
            if(cur->random != nullptr)
                cur->next->random = cur->random->next;
            cur = cur->next->next;
        }
        // 3. 拆分两链表
        cur = head->next;
        Node* pre = head, *res = head->next;
        while(cur->next != nullptr) {
            pre->next = pre->next->next;
            cur->next = cur->next->next;
            pre = pre->next;
            cur = cur->next;
        }
        pre->next = nullptr; // 单独处理原链表尾节点
        return res;      // 返回新链表头节点
    }
};
```