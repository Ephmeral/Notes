#! https://zhuanlan.zhihu.com/p/656227418
# CMU15-445 23fall-Project0
## 前言

2023 fall 课程已经开始了，听说今年又要添加新的内容，这次打算继续刷一遍 lab，其实去年写过 22fall 版本的，依稀记得被 B+ 树支配的恐惧，我可能前前后后调试了一个月左右（太菜了），最后 project4 也是差了一个测试用例没通过。所以今年的课打算重新刷一遍，顺便补充一些数据库的理论知识。

23fall 的 p0 和 23spring 的应该是一样的，不过今年的新加了一个 [15-445 Bootcamp](https://github.com/cmu-db/15445-bootcamp) ，这个是一些 Modern C++ 的用法，不熟悉 C++ 的可以先学一下这个。

p0 是需要实现一个基于 copy-on-write 字典树的 kv 存储，不熟悉字典树的可以先通过力扣这道题来熟悉一下 [208. 实现 Trie (前缀树)](https://leetcode.cn/problems/implement-trie-prefix-tree/)。这里为了方便将 key 统一为字符串类型，而 value 可以是任意类型。

键的最后一个字符对应的节点，存放了值。例如，考虑将 kv 对 ("ab", 1) 和 ("ac", "val") 插入到 trie 中，可以看到最后的节点分别存放了 1 和 val。

![Image](https://pic4.zhimg.com/80/v2-42a6efbc654e8ee8c12773b785212ed0.png)

## Task #1 - Copy-On-Write Trie

这个 task 是需要在 `trie.cpp` 和 `trie.h` 文件中实现 cow 字典树。cow 字典树任何操作都不会对原来的节点进行操作，每次插入或删除节点的时候都需要拷贝一份新的 TrieNode。

比如在上面例子中，如果我们想继续插入 ("ad", 2) 键值对的话，我画了一张图，详细的过程应该是：
- 先拷贝 root 节点创建一个 new_root，这个时候 new_root 和 root 共享着指向孩子节点的指针；
- 找到 new_root 的孩子 'a' 对应的节点 Node1，再将 Node1 拷贝一份得到 Node2，同样的 Node2 和 Node1 有着相同的指针分别对应着 'b' 和 'c' 的节点；
- 继续遍历键，发现要插入 'd'，这个时候因为 Node2 指向 'd' 的指针，所以需要创建一个新的节点。

<!-- ![](pics/Pasted%20image%2020230906175231.png) -->

![Image](https://pic4.zhimg.com/80/v2-3539ec232ce14c79682321251fa62107.png)

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

上面代码还有一个特殊情况没有考虑到，就是 key 为空的时候，相当于在 root 的位置上放入一个新的值，我在处理代码的时候枚举字符串的时候会留最后一个，如果 key 为空的话需要额外判断一下，单独处理一下就好。

2. Remove 函数

- 如果 root_ 是 nullptr 的话，直接返回，因为此时无法删除任何节点；
- 如果 key 是空的话，可能存在 ("", val) 这样的值，所以需要将 root_ 节点 `Clone()` 一份，相当于是将 TrieNodeWithValue -> TrieNode，主要还需要设置 is_value_node_ 为 false；
- 除了上面两种特殊情况，下面就是主要的实现逻辑，因为还要求当一个节点没有任何孩子节点的时候，这个节点就会被删除，这就可能出现递归删除父节点的情况。所以需要保存路径上的父节点，我这里使用 vector 来存储。
	- 用一个指针 prev 指向根节点，然后枚举 key 所有的字符 c，每次循环中查看 prev 中是否有字符 c 对应的孩子节点，如果没有直接返回。有的话需要将孩子节点复制一份，同时修改 prev 中的 `children_[c]`，还需要保存 prev 到父节点 vector 中；
	- 枚举完成后，复制最后一个节点，主要复制的时候返回 `std::shared_ptr<TrieNode>`，并修改 is_value_node_ 变量，同样的这个父节点也需要加入到 vector 中；
	- 最后从后往前将 vector 中的父节点取出来，判断它的孩子节点的 children_ 是否为空，如果是的话将父节点的 children_[c] 删除，相当于将孩子节点删除了；

3. Get 函数

Get 函数实现起来相对简单点，只需要从根节点开始，枚举 key ，依次看是否有对应的孩子节点，没有直接返回 nullptr。到最后一个节点的时候，判断是否为 is_value_node_，不是的话直接返回 nullptr。

最后需要将 `std::shared_ptr<const TrieNode>` 转换为 `const TrieNodeWithValue<T> *`，因为不能直接对智能指针进行 dynamic_cast，需要通过 shared_ptr 的 get 成员函数获得原始指针，然后再进行转换。最后返回的时候，将也是需要将 TrieNodeWithValue 的 value_ 通过 get 成员函数返回原始指针。

## Task #2 - Concurrent Key-Value Store

这个 task 是在 Trie 基础上加上并发，并发的要求是可以有多个读操作（Get 函数），只能有单个写操作（Put/Delete 函数）。另外写的时候，不会影响到读，这相当于读操作的时候是对旧 Trie 进行读的，而写的时候是对新的 Trie 进行写的。

TrieStore 有两个锁:
- root_lock_：用来保护 root_，即每次需要读或者写 root_ 的时候，都需要获得这把锁；
- write_lock_：用来同步读操作，最多只有一个读操作；

对于并发的 TrieStore 来说，写操作分为 3 个步骤：复制一份旧的 root_，在新的 root 上面进行写操作，然后再将新的 root 写回 root_，写操作必须用 write_lock_ 来进行同步，保证一次最多只有一个写，而只有对 root_ 进行操作就需要用 root_lock_ 来保护。

读操作只需要拷贝一份旧的 root_，然后在拷贝的 root 上面进行查找，最后返回的也是拷贝得到的 root。读操作不需要 write_lock_ 进行同步，但是仍然需要 root_lock_ 来保护 root_。

## Task #3 - Debugging

task3 我用的 Clion 调试，应该是最容易上手的，不算难。
## Task #4 - SQL String Functions

task4 其实就是实现一个大小写转换的函数，主要是找到对应的文件，跟着注释修改基本没啥大的问题。可能就是这两个函数不太好找，可以使用 `find . | grep filename` 来查询：

```shell
./src/include/execution/expressions/string_expression.h
./src/planner/plan_func_call.cpp
```

## 小结

这次代码写起来相对去年写的要快点，主要是缕清楚思路，代码实现的时候，可以多 review 一下，把一些地方能简化的尽可能简化一下。

PS:今年好像 23fall 的 gradescope 中 p0 好像没有测试代码了，不过可以找到 23spring 的 gradescope 提交一下，这两个是一样的。