---
title: "CMU15-445: Project0-字典树"
date: 2022-09-16T17:46:55+08:00
lastmod: 2022-09-19T16:57:36+08:00
tags: [CMU15-445]
categories: [Database, Lab]
from: 
image-auto-upload: true
---
2022/09/16 完成 大约7h

![](https://silas-py-oss.oss-cn-chengdu.aliyuncs.com/img/202209161747836.png)

后注：gradescope上面只能通过很少的测试用例，代码漏洞比较多，需要修改

2022/09/19 P0正式完结 总共用时大约12.5h

![](https://silas-py-oss.oss-cn-chengdu.aliyuncs.com/img/202209191643517.png)

PS：一直纠结后面的remove和insert函数的实现，结果发现最开始的HasChildren写错了。。。

## 前言

PROJECT #0 - C++ PRIMER：是要求实现一个支持并发插入键值对的字典树，如果堆字典树不了解可以先看一下 Leetcode 这道题 [208. 实现 Trie (前缀树)](https://leetcode.cn/problems/implement-trie-prefix-tree/)

写这个 project 一定要好好看看清楚给的提示以及代码当中的注释，写的很详细，根据这些各个函数的功能基本上可以实现。可能需要注意的就是一些 C++11 的语法，像智能指针、移动构造、std::move 函数等这些，如果不了解的话比较难上手，建议提前稍微了解一下，关于并发其实就是上个锁即可，并不涉及比较高级的技巧，这个不需要太担心

## 一些问题
1. 首先是移动构造

最开始我也不是很熟悉，但是我把涉及移动构造的都用 std::move 函数进行转移所有权（应该是这样），然后就没啥大问题了，使用 std::move 之后原来的变量就不存在了，不能继续使用了

2. 关于Trie::Insert() 这个函数

最开始理解错了，把每一步都由 TrieNode 转换为 TrieNodeWithValue 了，其实只要到最后一个字符对应的 TrieNode 看看是否需要转换即可
	 
3. 关于 TrieNodeWithValue 和 TrieNode 的转换
    
这个是运行时多态，可以动态绑定，具体转换我是创建一个新的变量，然后用 std::move 转移所有权（也可以用 std::make_unique 函数直接创建）

4. 关于 Trie::Remove() 函数
   个人感觉用迭代会简单点，递归不太好想，而且工程上尽量避免递归吧，可以用 vector 存放父节点模拟一下栈（这里的思路主要参考自[这个博客](https://www.cnblogs.com/alyjay/p/16698477.html)
   
5. 关于 Trie::GetValue() 函数

   最开始写错了，类型不匹配的时候 success 也设置成 true 了

6. 最后的最后卡了我好久的小bug

一直以为我的 Remove 写错了，直到后来发现是 TrieNode::HasChildren() 这个函数实现错了，其实本来很简单，我写成了遍历26个字母分别调用 HasChild 这个函数，后来发现不只是26个字母。。。还有数字啥的，这个错误导致我卡了两三天，一定一定要仔细看提示和代码注释

## 小Tips
本地测试会比gradescope 测试少很多，如果想知道gradescope上面的测试代码，可以根据提交之后上面会显示测试代码的路径，这个时候可以把代码打印出来，就可以知道测试代码了

下面是我打印测试用例的代码，文件名换成其他的应该也可以把其他 project 测试用例打印出来

```cpp
std::string buffer;
std::ifstream in_file("/autograder/bustub/test/primer/grading_starter_trie_test.cpp");
if (in_file.is_open()) {
  while (!in_file.eof()) {
	getline(in_file, buffer);
	std::cout << buffer << std::endl;
  }
}
in_file.close();
```

## 完结证明

![](https://silas-py-oss.oss-cn-chengdu.aliyuncs.com/img/202209191643517.png)

写于：2022/09/19 总共实验用时大约12.5h
