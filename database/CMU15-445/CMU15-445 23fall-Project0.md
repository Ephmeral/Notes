## 前言

2023 fall 课程已经开始了，听说今年又要添加新的内容，这次打算继续刷一遍 lab，其实去年写过 22fall 版本的，依稀记得被 B+ 树支配的恐惧，我可能前前后后调试了一个月左右（太菜了），最后 project4 也是差了一个测试用例没通过。所以今年的课打算重新刷一遍，顺便补充一些数据库的理论知识。

23fall 的 p0 和 23spring 的应该是一样的，不过今年的新加了一个 [15-445 Bootcamp](https://github.com/cmu-db/15445-bootcamp) ，这个是一些 Modern C++ 的用法，不熟悉 C++ 的可以先学一下这个。

p0 是需要实现一个基于 copy-on-write 字典树的 kv 存储，不熟悉字典树的可以先通过力扣这道题来熟悉一下 [208. 实现 Trie (前缀树)](https://leetcode.cn/problems/implement-trie-prefix-tree/)。这里为了方便将 key 统一为字符串类型，而 value 可以是任意类型。

键的最后一个字符对应的节点，存放了值。例如，考虑将 kv 对 ("ab", 1) 和 ("ac", "val") 插入到 trie 中，可以看到最后的节点分别存放了 1 和 val。

![15445.courses.cs.cmu.edu/fall2023/project0/trie-01.svg](https://15445.courses.cs.cmu.edu/fall2023/project0/trie-01.svg)


## Task #1 - Copy-On-Write Trie

这个 task 是需要在 `trie.cpp` 和 `trie.h` 文件中实现 cow 字典树。cow 字典树任何操作都不会对原来的节点进行操作，每次插入或删除节点的时候都需要拷贝一份新的 TrieNode。

比如在上面例子中，如果我们想继续插入 ("ad", 2) 键值对的话，我画了一张图，详细的过程应该是：
- 先拷贝 root 节点创建一个 new_root，这个时候 new_root 和 root 共享着指向孩子节点的指针；
- 找到 new_root 的孩子 'a' 对应的节点 Node1，再将 Node1 拷贝一份得到 Node2，同样的 Node2 和 Node1 有着相同的指针分别对应着 'b' 和 'c' 的节点；
- 继续遍历键，发现要插入 'd'，这个时候因为 Node2 指向 'd' 的指针，所以需要创建一个新的节点。

![](pics/Pasted%20image%2020230906175231.png)

删除一个键值对和上面的过程基本类似，遍历的每一个节点都需要新拷贝一份，然后再进行操作。

下面介绍一下代码实现思路，为了尊重 CMU 课程学术要求，这里不会贴上我的代码。

1. Put 函数

Put 函数的实现就是按照我上面说的：
- 首先创建一个 `std::shared_ptr<TrieNode> new_root` 指针，因为原 Trie 的 root_ 是指向 const TrieNode 的，是不可以直接修改的，但是通过调用 `Clone()` 方法，我们可以得到 `std::unique_ptr<TrieNode>` ，再将 unique_ptr 指针转为 shared_ptr 指针就可以继续操作了。注意如果 Trie 的 root_ 是空的话，直接创建一个新的 `std::shared_ptr<TrieNode>`；
- 枚举字符串 key，注意最后一个字符需要额外进行操作，枚举的时候，每次通过 new_root 中的 map 来查找对应的字符 c 是否存在，如果存在的话，`Clone()` 一份孩子节点，如果不存在创建一个新的空节点。然后将 new_root 的 `children_[c]` 改成这个新的孩子节点，最后让 new_root 指向字符 c 对应的孩子节点，依次遍历；
- 最后一个字符对应的节点，可能有两种情况：
	- 这个节点原来也存在，不管原来的值是多少，仍然通过 `Clone()` 拷贝一份孩子节点，这里需要注意的是 `TrieNodeWithValue<T>` 它的构造函数时通过移动来完成的，所以对于值 value 来说需要用 `std::move(value)`；
	- 这个节点不存在，创建一个带有值的新的节点 `TrieNodeWithValue<T>`；
	- 创建了最后的孩子节点之后，修改父节点的 `children_[c]`；
	- 上面创建新的节点和拷贝节点，都可以用 `std::make_shared` 函数来完成；
- 最后返回的时候，需要新创建一个 Trie，这样的话，最开始创建的 new_root 就需要保存一下；

上面代码还有一个特殊情况没有考虑到，就是 key 为空的时候，相当于在 root 的位置上放入一个新的值，

## Task #2 - Concurrent Key-Value Store

## Task #3 - Debugging

## Task #4 - SQL String Functions

