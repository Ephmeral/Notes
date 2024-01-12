# STL标准库与泛型编程
## 1. 认识 headers、版本、重要资源

版本：VS C++6.0、Dev-C++5.11 with GNU4.9.2  
主要网页：
- cplusplus.com
- cppreference.com
- gcc.gnu.org  
书籍：
- The Standard Library C++11
- STL 源码剖析
## 2. STL 体系结构基础介绍
#### STL 六大部件

<img src="https://silas-py-oss.oss-cn-chengdu.aliyuncs.com/img/202211232113647.png"/>

<img src="https://silas-py-oss.oss-cn-chengdu.aliyuncs.com/img/202211232114375.png"/>

#### 常见的复杂度：
- $O(1)$ 或 $O(c)$：常数时间
- $O(n)$：线性时间
- $O(\log_2 n)$：对数时间
- $O(n^2)$：平方时间
- $O(n^3)$：立方时间
- $O(2^n)$：指数时间
- $O(n \log_2 n)$：介于线性和二次方之间

#### 前闭后开区间

<img src="https://silas-py-oss.oss-cn-chengdu.aliyuncs.com/img/202211232118257.png"/>

C++11 之后可以使用 for range 遍历容器：

```cpp
/*
for (delc : coll) {
	statement
}
*/
for (int i : {2, 3, 4, 5, 9}) {
	std::cout << i << std::endl;
}

std::vector<double> vec;
...
// C++11之后：范围遍历
for (auto elem : vec) {
	std::cout << elem << std::endl;
}
// 使用auto& 表示引用，相当于给容器中的元素起个别名
// 后续的操作直接对容器中的元素进行
for (auto &elem : vec) {
	elem *= 3;
}
```

auto 关键字：可以自动推导类型

```cpp
// C++11之前的写法
list<string> c;
...
list<string>::iterator it;
it = ::find(c.begin(), c.end(), target);

// C++11之后可以用auto直接推导类型名字
list<string> c;
...
auto it = ::find(c.begin(), c.end(), target);
```

## 3-7. 容器分类和各种测试

容器分类：
- 序列式容器：array、vector、deque、list、forward_list
- 关联式容器：set/multiset、map/multimap、unordered_set/multiset、unordered_map/multimap

<img src="https://silas-py-oss.oss-cn-chengdu.aliyuncs.com/img/202211232127988.png"/>

- 红色部分是 C++11 之后出现的
- set/map 等内部原理一般是用红黑树实现的
- unordered 容器一般是用哈希表实现的

下面是一些测试代码，这里就不粘贴了，可以看下课件和提供的源代码

## 8. 源代码分布 (VC, GCC)

**源码之前，了无秘密**

- VC 源代码在 `include` 文件夹中
- GNU C++ 通过 Dev C++ 编译器可以找到在 `..\4.9.2\include` 文件夹有源代码
>注：感觉现在用 VScode 配好环境之后比较方便

## 9. OOP vs GP

OOP：Object-Oriented programming 面向对象编程  
GP：Generic Programming 泛型编程

![](pics/Pasted%20image%2020230818162115.png)

- OOP 会将数据和方法关系在一起，标准库中的 list 是单独有个 sort 函数，而不是使用全局函数；
- 为什么不能直接使用 ::sort 进行排序？因为 ::sort 只支持 RandomIterator，而 list 并不是（后续会介绍）；

![](pics/Pasted%20image%2020230818162643.png)

- GP 是将数据和方法分开，而迭代器则是将对象和方法联系到一起的桥梁；
- 通过迭代器可以对容器中的数据进行排序等操作；

![](pics/Pasted%20image%2020230818162950.png)

- GP 的用途在于可以对函数进行重载，生成不同类型的函数；
- 比如上面的 min 和 max，这里考虑到类型 T 可能并没有重载 < 号，所以也可以自定义比较方法，即下面的传入个 Compare；

![](pics/Pasted%20image%2020230818163416.png)

- 所有的算法 algorithms 最终到涉及到元素本身的操作，比如说比大小；
- 左边的例子中，strLonger 用来比较两个字符串的长度，使用 max 默认的比较是根据字典序进行比较。而自定义 Compare 为 strLonger 的时候比较的就是字符串的长度了；

## 10. 操作符重载 & 模版
### 操作符重载
cppreference 说明了哪些操作符可以进行重载：[operator overloading - cppreference.com](https://en.cppreference.com/w/cpp/language/operators)

![](pics/Pasted%20image%2020230818164201.png)

- 上面的表格展示了对于一元操作符、二元操作符，以及其他的赋值操作符等等，当它们作为成员函数和非成员函数的时候，具体的写法；

![](pics/Pasted%20image%2020230818164342.png)

- 这里的例子是链表的迭代器，左下角圈出来的是具体的操作符重载的函数命名写法；
### 类模版

![](pics/Pasted%20image%2020230818164518.png)

- 上面定义一个复数的模版，模版在 class 的前面加上 `template<typename T>` 其中 T 指代的是类型，类中可以直接使用 T 作为类型使用；
- 定义一个类模版对象的时候，需要在类名后面加上具体的类型，以实例化具体的对象；
- 更多模版的知识可以参考 C++ Templates 这本书；

![](pics/Pasted%20image%2020230818164826.png)

- 函数模版，前面已经看过了，在定义函数的时候前面加上和类模版类似的 `template<typename class T>`；
- 注意这里 typename 后面的 class 和 typename 并没有区别，相比而言 typename 的使用范围更广（即还有其他用途）；
- 上面例子中，定义了 stone 类，在调用 min 函数的时候，b < a 这里会查看 stone 类有没有重载 < 运算符，发现重载了就会实际上调用 `stone::operator<()` 这个函数；
- 可以发现使用函数模版的时候，不需要像类模版那样用尖括号写上类型名字，因为函数模版调用的时候会自动根据对象进行推导类型；

![](pics/Pasted%20image%2020230818165420.png)

- 成员模版就是成员函数上定义成为模版函数，这样可以在成员函数中使用其他的类型；
### 特化

![](pics/Pasted%20image%2020230818165515.png)

- 前面介绍的模版是用来泛化类型的，用 T 来替代任意的类型，如果在定义这个类的时候有个特例，比如对于鸟来说，可能会定义一个会飞的鸟类，但是企鹅也是鸟却不会飞，这个时候我们需要写出这个特例，这种叫做模版的特化；
- 特化的具体写法是，用 `template<>` 而不是 `template<typename T>` ，另外在类名后面加上尖括号和类型，表面是具体哪个类型的特化版本；
- 上面的例子中就是对 `__type_traits` 的 int 和 double 类型的特化；

![](pics/Pasted%20image%2020230818170147.png)

- 另外一个例子是 hash 结构体，对 char、short、int 等等类型进行特化；

![](pics/Pasted%20image%2020230818170247.png)

- 这也是另外一个特化的例子，是对 void 类型进行特化；

上面说的特化，又被称为是全特化，另外一种是偏特化：

![](pics/Pasted%20image%2020230818170402.png)

- 左边的例子中，vector 有两个模版参数 T 和 Alloc，如果只特化参数 T 为 bool 类型，而 Alloc 仍然作为模版参数，此时将其称为偏特化；
- 右边的例子中，定义了泛化的 iterator_traits 类，模版参数为 Iterator，但是如果是指针类型或 const 指针类型，也是一种偏特化；
## 11. 分配器

关于 STL 的分配器，在内存管理这门课上讲解的还算详细，可以看下笔记 [内存管理#G2.9 alloc 运行模式|内存管理](05-C++内存管理机制.md)
## 12. 容器分类

![](pics/Pasted%20image%2020230820210134.png)

- 容器分为序列式容器：array, vector, deque, list, forward_list
- 关联式容器：set/multiset, map/multimap
- 无序容器：unordered_set/multiset, unordered_map/multimap

![](pics/Pasted%20image%2020230820210315.png)

- 这页是对前面的分类具体细化了，这里以缩进的形式来表示衍生关系；
- 比如 heap 中有一个 vector 做支持，priority_queue 中有一个 heap，stack 中有一个 deque 做支撑，set、map 等内部是有一个 rb_tree 做支撑；
- 另外 C++11 之后对标准库容器修改了名字，slist 改为 forward_list，hash_set/map 等改为 unordered_set/map；

## 13-14. 深度探索 list

![](pics/Pasted%20image%2020230820213020.png)

- list 容器中有一个 `__list_node` 节点，它有两个指针，分别指向前面和后面的节点，另外有一个 T 类型的变量 data；
- list 容器还有一个迭代器，详细内容下面分析；
- list 容器会刻意的尾端加上一个空白节点，以满足**前闭后开**区间；

![](pics/Pasted%20image%2020230820213404.png)

- list 的迭代器需要定义 5 种类型（后面详细介绍迭代器的时候会知道用途）；
- list 迭代器还需要重载一系列运算符，以模拟指针的形式；
- list 迭代器本身也包含一个 `__list_node` 节点，有了它才能找到前后节点，以及数据；

![](pics/Pasted%20image%2020230820213652.png)

- 迭代器最重要的就是 ++ 操作，我们知道对于基本类型（如 int）是分为前 ++ 和后 ++ 的；
- 前 ++ 是先将值加 1，再返回新的值，返回的值是一个左值，是可以支持 ++++i 的；
- 后 ++ 是返回原来的值，然后在将值加 1，返回的值是一个右值，不能 i++++；
- 所以对于迭代器来说，需要模拟这个特性，`operator++(int)` 代表后 ++，`operator++()` 代表前 ++，具体实现可以看下上面的代码；

![](pics/Pasted%20image%2020230820214409.png)

- list 迭代器另外需要重载的操作符是 `*` 和 `->` 即解引用和访问对象的成员函数；
- `operator*()` 返回节点数据的引用，`operator->()` 则是调用了 `operator*()` 后再取地址；
- `->` 运算符实际上是对 `*` 和 `.` 运算符的组合使用；

![](pics/Pasted%20image%2020230820214922.png)

- 到了 G4.9 将迭代器一些不合理的地方修正了，比如在 typedef 的时候只需要传入一个类型就可以了，而引用和指针类型都是可以根据 T 得到的；
- 另外在 `__list_node` 结构体中，prev 和 next 指针是 void 类型，使用的时候还需要强转一下，到了 G4.9 就将其分离出一个 `_list_node_base` 指向这个节点类型；

![](pics/Pasted%20image%2020230820215235.png)

- G4.9 的 list 相比 G2.9 继承关系要复杂的多，但是对于链表整个结构和原来一样；
- 每个节点都有两个指针，指向前后节点，最后仍然有一个空白节点，用来满足前闭后开区间要求；
- 当调用迭代器 end() 的时候，返回的是最后一个节点的下一个节点（图中蓝色的节点）；

## 15. 迭代器设计原则和 Interator Trait

![](pics/Pasted%20image%2020230821135922.png)

- 迭代器 Iterator 是算法和容器之间的桥梁，使用算法的时候，需要向迭代器提问，迭代器需要回答，以便算法能够根据迭代器的类型做出对应的选择；
- rotate 函数实际上调用了 `std::__iterator_category` 来获取迭代器的类型，即 `iterator_category`，这里迭代器返回的类型是 `random_access_iterator_tag`，因此调用对应的函数；
- 此外还需要迭代器回答 `difference_type` 和 `value_type` 类型，C++ 标准库总共设计了 5 种需要迭代器回答的类型；

![](pics/Pasted%20image%2020230821140642.png)

- 这里列出来 Iterator 必须提供的 5 种 associated types，有了这些类型，当算法进行提问的时候，迭代器就能够回答这些类型是什么；
- 所谓提问，即将使用 `I::iterator_category` 形式将迭代器相应的类型提取出来；
- 这里的问题是，如果 Iterator 本身不是 class 呢？例如原生指针，它也是可作为迭代器的，但是指针没法 typedef， 如何解决这个问题？

![](pics/Pasted%20image%2020230821141456.png)

- 前面提到对于原生指针来说，没有直接回答 associated types，这里解决的办法是加一个中间层，有了这个中间层，当传入的迭代器是指针类型的时候，也能做出相应的回答；

![](pics/Pasted%20image%2020230821141753.png)

- Iterator Traits 的具体实现，是定义了一个模版类 `iterator_traits`，如果是迭代器类型，会向迭代器询问 value_type 是什么；
- 而如果是指针，这里用到了模版偏特化的特性，将指针类型单独拎出来，指针类型返回的 value_type 返回的就是类型 T 本身；
- 所以当算法向要知道迭代器的 value_type 的时候，会向 `iterator_traits` 询问，而 `iterator_traits` 根据模版的特性会选择使用的是 Iterator 类型还是指针类型；
- 这里注意的是对于指向 const 的指针，它的 value_type 返回的也是类型 T，而不是 const T，因为 value_type 主要用来声明变量，而 const 变量的话无法被赋值，作用不大；

![](pics/Pasted%20image%2020230821142936.png)

- 这页展示了完整的 5 种类型，指针的 `iterator_category` 是 `random_access_iterator_tag`

标准库中还有各种各样的 traits

![](pics/Pasted%20image%2020230821143416.png)
## 16. vector 深度探索

![](pics/Pasted%20image%2020230822100648.png)

- vector 用的是一段连续的空间，相比于原生数组来说，vector 可以动态调整大小；
- vector 类中有三根指针：
	- start ：指向内存中起始的位置；
	- finish ：指向已经使用空间的末尾；
	- end_of_storage ：指向可用空间的末尾；
	- 这里注意 finish 和 end_of_storage 的区别，前者指向已经使用的空间，而后者则是整个 vector 的可用空间的末尾；
- vector 的 size 是元素的个数，而 capacity 是容器的所有可用空间；
- 当容器满的时候，会进行扩容，不同编译器会有所区别，一般为 2 倍增长；
- 左边还展示了 vector 的一些成员函数，应该比较好理解；

![](pics/Pasted%20image%2020230822101810.png)  
![](pics/Pasted%20image%2020230822101839.png)

- vector 的 push_back 函数是在容器的尾部添加一个新的元素；
- 如果 vector 的空间还充足，就会在末尾构造一个新的元素；
- 而如果空间不足，会调用 `insert_aux` 函数：
	- 这个函数首先会检查空间大小是否充足，充足的话在插入的位置上构造一个新的元素（这里和前面的 push_back 检查是重复的，因为 `insert_aux` 还会被其他函数，如 insert 调用，所以检查是有必要的）；
	- 如果空间不足了，会进行扩容，扩容的时候计算出新的大小（2 倍增长），然后调用分配器分配一块内存，然后将容器原来的内容都拷贝到新的内存中，注意这里也会拷贝 position 后面的内容（同上，insert 调用的时候，拷贝插入位置后面的元素是有必要的）；
	- 拷贝完成之后，会调用析构函数释放原来的元素，并用分配器回收原来的内存空间；
- 扩容的时候，这里有个问题是**会大量的调用构造函数和析构函数**；

![](pics/Pasted%20image%2020230822102732.png)

- vector 的迭代器使用的就是原始的指针，得益于 traits 技巧，当算法在使用 vector 迭代器的时候，会使用 iterator_traits 的偏特化版本；

![](pics/Pasted%20image%2020230822103016.png)  
![](pics/Pasted%20image%2020230822103246.png)

- G4.9 版本相比 G2.9 版本而言，类的关系要复杂的多，绕了一大圈其实和 G2.9 版本没多打区别；

## 17. array, forward_list

![](pics/Pasted%20image%2020230824101426.png)

- array 容器是对原生数组进行了简单的包装，它的大小是固定的，在声明的时候需要指定大小，例如 `array<int, 10> nums` 声明了 int 类型的数组，数组大小为 10；
- array 内部实现的时候，就是声明了一个 `_Nm` 大小的原生数组，如果 array 声明大小为 0 的话，会自动调整为 1；
- array 的迭代器使用的就是原生指针，iterator traits 机制前面介绍过了；

![](pics/Pasted%20image%2020230824102037.png)

- 上面是 TR1 版本的 array，在 G4.9 版本中，变得更加复杂了，但是本质都是一样的，对原生数组进行包装；

![](pics/Pasted%20image%2020230824102115.png)

- forward_list 是单向链表，forward_list 的迭代器是单向的，其他和前面的双向链表 list 原理差不多，这里不过多介绍；

## 18-19. deque 深度探索

![](pics/Pasted%20image%2020230824103222.png)

- deque 容器是一个双端队列，对于 vector 来说只能向后扩充，而 deque 既可以向后也可以向前扩充元素；
- deque 容器在使用上就像左上角是一段连续的空间，可以在前后添加元素，而实际上是分块的，每一块作为一个缓存区 buffer；
- deque 有一个 map 来管理这个缓冲区（map 类型是数组，其中数组指向了一块 buffer），当需要往后添加元素的时候，先在 buffer 中添加，当 buffer 满了，会在 map 的后面新添加一块缓冲区 buffer 在添加元素。向前添加元素同理；
- deque 的迭代器它有 4 根指针，其中 node 指向当前迭代器所在的缓冲区 buffer， first 和 last 指向 buffer 的起始和终止位置，另外的 cur 则是对应当前迭代器指向的那个位置；
- 对于迭代器来说，往后遍历的时候，当移动到最后一个元素的时候，会进 node 指向下一块 buffer，同时 first 和 last 都需要改变，往前遍历同理；

![](pics/Pasted%20image%2020230824104059.png)

- 这里展示了 deque 的源代码，deque 模版参数有 3 个分别为类型 T，分配器 Alloc，以及大小 BufSiz，这里的 BufSiz 是可以自定义的，它的内部有个 `__deque_buf_size` 函数用来计算 buffer 的大小的：
	- 如果 n 不为 0，则 buffer 大小是用户指定的；
	- 如果 n 为零，buffer 大小使用预设值，即看下 `sizeof(value_type)` 的大小 sz：
		- 如果 sz 小于 512，则返回 512 / sz；
		- 如果 sz 不小于 512，则返回 1；
- deque 有 2 个迭代器 start 和 finish 分别指向起始和结束的位置；
- map 是一个 T** 类型，map_size 是 size_type 类型，所以 deque 容器本身的大小是 40 字节（两个迭代器各为 16 字节，迭代器各有 4 个指针，map 也是指针即 4 字节，map_size 也是 4 字节，所以 16 + 16 + 4 + 4 = 40），注意这里计算是按照 32 位计算的，deque 容器的元素也是不包含进来的；

![](pics/Pasted%20image%2020230824105751.png)

- deque 迭代器定义了规定的 5 中类型，迭代器有 4 个指针，前面介绍过了；

![](pics/Pasted%20image%2020230824105943.png)  
![](pics/Pasted%20image%2020230824110449.png)

- deque 中 insert 函数在迭代器 position 位置上，插入 x；
- 如果插入的是 deque 最前端，会交给 `push_front()` 操作；
- 如果插入的是 deque 最尾端，会交给 `push_back()` 操作；
- 否则的话，会调用 `insert_aux()` 函数：
	- 先计算在 pos 位置上插入之前的元素个数有多少；
	- 如果比 deque 容器的一半还小，说明插入位置之前元素更少，在最前端加入和第一个元素同值的元素，然后元素向前搬移，空出一格；
	- 否则在最尾端插入与最后一个元素同值的元素，然后元素向后搬迁，空出一格；
	- 在插入的位置上设置新的值；
	- 即完成了在 pos 上插入 x 的操作；

![](pics/Pasted%20image%2020230824125028.png)

- deque 在使用上好像是连续空间，实际上都是 deque 的迭代器做了很多工作；
- 这里的几个函数都是 deque 的成员函数：
	- `operator[]` 是对中括号重载，返回的是 deque 中第 n 个元素，实际上会调用迭代器的 `operator[]` 成员函数；
	- `front()` 返回的是开始的元素；
	- `back()` 返回的是最后一个元素，这里一个迭代器 finish 指向的是最后一个元素的下一个元素，所以要返回它的上一个元素；
	- `size()` 返回的是元素个数，这里的 finish - start 中减号会调用迭代器的操作符重载函数 `operator-()`；
	- `empty()` 表示 deque 是否为空，同样 == 也是调用了迭代器的操作符重载函数 `operator==()`；

![](pics/Pasted%20image%2020230824130449.png)

- 这里是迭代器的一些操作符重载；
- `operator*()` 返回的是当前元素，就是对 cur 指针进行解应用；
- `operator->()` 返回当前元素的指针类型，调用了 `operator*()` ，再使用取地址符号，返回元素的地址；
- 重点是 `operator-()` ，它先计算两个迭代器之间有多少个完整的节点 node，乘以 buffer_size() 即可得到有多少个元素，然后加上起始 buffer 中的元素个数，以及末尾（当前）buffer 的元素个数，就可以计算得到两个迭代器之间有多少个元素了；

![](pics/Pasted%20image%2020230824131055.png)

- `operator++()` 是前 ++ 的操作符重载，`operator++(int)` 是后 ++ 的操作符重载，-- 同理；
- `operator++()` 函数先将迭代器中 cur 指针往后移动一格，如果发现 cur 到达 last 的位置时候，需要将整个迭代器移动到下一个节点的开始位置上；
- `operator++(int)` 调用了前 ++ 的操作符重载函数；
- `operator--()` 函数先判断当前位置 cur 是否在缓冲区的起始位置，如果是的话将迭代器移动到上一个节点的 last 位置上，然后再往前移动一格位置；（这里因为 last 指向的是最后一个元素的下一个位置，所以 --cur 之后是最末尾的元素）；
- `operator--(int)` 同理，调用了前 -- 的操作符重载函数；

![](pics/Pasted%20image%2020230824132038.png)  
![](pics/Pasted%20image%2020230824133241.png)

- 上页的迭代器是每次加一个元素，对于一个 random 迭代器来说是可以加 n 个元素的；
- `operator+=(n)` 函数让迭代器一次能够加 n 个元素，首先会判断目标位置是否在同一个缓存区中：
	- 如果是的话，cur 指针加上 n 即可；
	- 如果不是的话，要计算出缓冲区的位置，切换到正确的缓冲区，再将 cur 指针改为正确的位置；
- `operator+(n)` 和 `operator-=` 直接复用了 += 操作符重载；

到了 G4.9 版本也是一样，类之间的关系变得复杂起来，这里不赘述了，因为原理都是一样的；

### queue + stack

![](pics/Pasted%20image%2020230824134412.png)  
![](pics/Pasted%20image%2020230824134538.png)

- queue 和 stack 内部使用了其他容器，他们本身只是对底层容器进行了简单的封装，他们实际上是适配器；
- 默认情况下使用的是 deque 作为底层容器，可以从模版参数的默认值上面看出来；
- 只要底层使用的容器有相应的操作，就可以作为 queue 和 stack 的容器；
	- vector 可以作为 stack 的容器，但是不能作为 queue 的容器；
	- list 可以作为 queue 和 stack 的容器；
	- set 和 map 都不可以作为 queue 和 stack 的容器；
- queue 和 stack 不允许遍历，也不能提供迭代器；

## 20. RB-tree 深度探索

Red-Black tree（红黑树）是平衡二叉搜索树中最常被使用的一种。平衡二叉搜索树的特性：排序规则有利于 search 和 insert，并保持高度平衡——无任何过深节点。

RB-tree 提供遍历操作和迭代器。按照正常规则（++Iter）遍历，能够获得排序状态。

不应该使用任何 rb_tree 的 iterators 改变元素的值

我们不应使用 rb_tree 的 iterators 改变元素值（因为会违背元素的排列规则）。编程层面**并没有阻止此事**。这样设计是正确的，因为 rb_tree 将为 set 和 map 服务（作为其底部容器支持），而 map 运行元素的 data 被改变，只有元素的 key 才是不可以被改变的。

rb_tree 提供两种 insertion 操作：`insert_unique()` 和 `insert_equal()`。前者表示节点的 key 在整个 tree 中独一无二，否则插入失败；后者表示节点的 key 可以重复。

![](pics/Pasted%20image%2020230826193525.png)

- rb_tree 是一个模版类，它的 5 个模版参数分别是：
	- Key 即红黑树的键；
	- Value 即红黑树的值，注意这里的值是 key | data 组合而成的，而不只是 data；
	- KeyOfValue 用来告诉如何从 Value 中取出 Key；
	- Compare 用来告诉 Key 如何比较大小的；
	- Alloc 分配器，和之前的容器一样；
- rb_tree 类中有 3 个成员变量：
	- node_count 指的是 rb_tree 的节点个数；
	- header 是一个指针，指向 rb_tree_node 即红黑树的节点类；
	- key_compare 是一个仿函数，用来比较 key 的大小；
- rb_tree 类本身的大小是 4 + 4 + 1 = 9（对齐为 12），仿函数是一个空的对象，C++ 标准规定一个空类大小是 1；
- 对于 rb_tree 来说，它有一个假想的节点，这个节点就是 header；

## 21. set/multiset 深度探索

set/multiset 以 rb_tree 为底层数据结构，因此有元素自动排序的特性。排序的依据是 key，而 set/multiset 元素的 value 和 key 是同一个，value 就是 key。

set/multiset 提供遍历操作和 iterators。按照正常规则（++ite）遍历，可以获得排序状态。

无法使用 set/multiset 的 iterators 改变元素值（因为是根据 key 进行排序的，一旦改变了 key，原来的顺序也就发生了变化）。set/multiset 的 iterator 是其底部容器 rb_tree 的 const_iterator，就是禁止了用户对元素赋值。

set 中元素的 key 是唯一的，insert() 用的是 rb_tree 的 insert_unique()。multiset 元素的 key 可以重复，因此用的是 rb_tree 的 insert_equal()。

![](pics/Pasted%20image%2020230829155236.png)

- set 容器有 3 个模版参数：
	- key 指的是键；
	- Compare 是仿函数用来比较大小，默认为 `less<Key>`；
	- Alloc 是分配器，默认为 alloc；
- set 重新定义了 rep_type 类型，其实就是对 rb_tree 类型，可以看到 key_type 和 value_type 都是 key 类型，另外 `identity<value_type>` 也是一个仿函数它返回的就是 key；
- 注意 set 的迭代器使用的是 rb_tree 中的 const_iterator，这也意味着用户不能使用迭代器改变 key；
- set 所有的操作都是转交该 rb_tree 进行的，set 也可以看成是容器适配器；

![](pics/Pasted%20image%2020230829160156.png)

- 前面用的 identity()，这个函数 VC 当中并没不提供，它是用了另外的一个函数对象 `_Kfn` 本质都是一样的，value 就是 key；

## 22. map/multimap 深度探索

map/multimap 以 rb_tree 为底层数据结构，因此有元素自动排序的特性。排序的依据是 key。

map/multimap 提供遍历操作和 iterators。按照正常规则（++ite）遍历，可以获得排序状态。

无法使用 map/multimap 的 iterators 改变元素值（因为是根据 key 进行排序的，一旦改变了 key，原来的顺序也就发生了变化），但是可以用来改变元素的 data（key 和 data 组成了 value）。因此 map/multimap 内部自动将用户指定的 key type 设为 const，这样就可以禁止用户对元素的 key 赋值。

map 中元素的 key 是唯一的，insert() 用的是 rb_tree 的 insert_unique()。multimap 元素的 key 可以重复，因此用的是 rb_tree 的 insert_equal()。

![](pics/Pasted%20image%2020230829160910.png)

- map 有 4 个模版参数：
	- Key 指的是键的类型；
	- T 指的是数据的类型（注意 data 和 value 类型不一致）；
	- Compare 是仿函数用来比较 key 的大小，默认是 `less<Key>`；
	- Alloc 是分配器，默认是 alloc；
- map 中的 value_type 是 `pair<const Key, T>` 类型，这意味着 key 是不能改变的；
- 同样 map 中也有一个 rb_tree 作为底层容器，rb_tree 的第三个模版参数 `select1st<value_type>` 根据名字能够知道，它是将 value_type 的第一个类型取出来；
- map 的迭代器是 rb_tree 中的 iterator，这意味着是可以修改迭代器的内容的，而因为 value_type 中 Key 是 const 的，所以还是不能修改 Key，但是可以修改 data；

![](pics/Pasted%20image%2020230829161949.png)

- 同样的在 VC6 中不提供 select1st() 仿函数，提供的是 `_Kfn` 也是返回 value_type 第一个类型；

![](pics/Pasted%20image%2020230829163050.png)

- map 容器中有个特殊的操作符重载 `operator[]`，它的作用是返回 key 对应的 data 类型的引用，如果 key 不存在的话会创建一个新的 key；
- 通过源代码可以看到 `operator[]` 先调用 lower_bound 进行二分查找，找到大于等于 key 的第一个迭代器，然后判断这个迭代器是否存在，不存在的话调用 insert 插入一个新的 key, mapped_type。
- 这启示我们如果知道要插入的元素是不存在的话，可以直接使用 insert，而不是调用 `operator[]`，虽然后者更加直观，但是效率会稍微慢一点。

## 23-24. hashtable 深度探索

![](pics/Pasted%20image%2020230831085244.png)

- 如果空间足够大的话，可以直接在 hashtable 中放下所有元素，但这往往是不现实的；
- 考虑有限的空间 M，将每个对象通过 hash 函数计算得到一个值，然后对 M 取模即可计算得到这个对象应该放入的位置；
- 但是这样难免会出现冲突（即不同对象计算得到的位置是同一个），产生冲突的解决方法是用其他方法重新计算对象的 hash 值，然后再放入桶中，这种方法如果冲突次数很多的话，就不太适用了，每次重新计算对象开销会很大，考虑下面的方法；

![](pics/Pasted%20image%2020230831085826.png)

- STL 中 hashtable 出现冲突采用的是拉链法，即将所有相同 hash 值的放在一根链表上，这样但出现冲突的时候只需要在链表上插入即可；
- hashtable 元素数量大于桶数组大小的时候，就会发生扩容，每次扩容大概是 2 倍；
- 扩容大小实际是根据 2 倍左右的质数来决定的，G2.9 初始桶大小为 53，之后每次扩容到 2 倍左右的质数，用质数的原因是能够减少碰撞的次数；

![](pics/Pasted%20image%2020230831091145.png)

- hashtable 类有 6 个模版参数：
	- Value 是节点的值类型；
	- Key 是节点的键类型；
	- HashFcn 是哈希函数，用来计算对象的哈希值的；
	- ExtractKey 从节点中取出键的方法，是一个函数对象；
	- EqualKey 函数对象用来比较 Key 是否相等；
	- Alloc 分配器；
- hashtable 类中有三个函数对象成员变量，就是上面提到的模版参数，它们各自的大小为 1（空类的大小为 1）；
- hashtable 还有一个 buckets 成员变量，它用的是 vector 容器，图示在右下角，可以看出是一个桶，每个桶当中可能不止一个元素，这些元素是通过链表串联起来的；
- 节点类型是 `__hashtable_node`，它有一个指针指向下一个节点，以及一个 value 成员变量，表示节点的值，节点是单向链表（在其他实现中可能是双向链表）；
- hashtable 的迭代器包含一个节点的指针以及 hashtable 的指针，有 hashtable 的指针是为了能够在遍历的过程中，从一个 bucket 跳到另外一个 bucket；

![](pics/Pasted%20image%2020230831093244.png)

- 这里实际演示了如果用户想用 hashtable 的话，该如何传入模版参数；
- 这里以 const char* 作为 Value 和 Key 的类型，哈希函数用的是 `hash<const char*>`（下页会说明），identity 表示键和值是同一个，eqstr 是用来比较字符串的仿函数；
- C 中提供了 strcmp 来比较字符串的实际内容是否一致，但是由于这里要求的接口不一致，strcmp 返回 -1 表示不一致，而模版参数要求不一致的时候返回 false，所以对 strcmp 进行了额外的封装；

![](pics/Pasted%20image%2020230831095503.png)

- 前面提到 hash 是一个函数对象，这里定义了泛化的 hash 结构体，然后对各个基本类型做特化，这样的话当使用 hash 这些基本类型的的时候就会使用特化版本的；
- 从上面可以看出对于 char，short，int 等类型，返回的就是元素本身，因为对于一个整数来说用它自己作为桶的位置是可以的，也是符合常识的；

![](pics/Pasted%20image%2020230831095937.png)
- 但是对于 const char* s 即 C 风格字符串来说需要考虑其他方法，这里用到了 `__stl_hash_string()` 函数，可以看到这个函数就是将字符串每个值取出来不断乘以 5 再迭代，这里具体为什么这样做的原因这里不讨论了；
- 注意在 G2.9 版本中没有对 C++ 风格的字符串进行特化，所以想在 hashtable 中存放 C++ 风格的字符串需要用户自定义对应的 hash 函数，而 G4.9 版本就提供了；

## 25-26. unordered 容器概念

好像少了一节视频，不过 PPT 里面也没见结束 hash_set/hash_map 相关的内容，可以看下书

![](pics/Pasted%20image%2020230901101733.png)

- 到了 C++11 之后 hash_set/map 等都改成了 unordered_ 开头，使用上和 set/map 几乎一样，但是内部原理确完全不一样，一个是散列表，一个是红黑树；

## 27. 算法的形式

![](pics/Pasted%20image%2020230901102408.png)

- STL 中的算法从语言层面来看是一个模版函数，STL 的其他组件则都是模版类；
- 算法的作用是操作容器中的数据，来完成一些特定的功能，比如排序、查找、排列组合等等。但是容器中的数据算法是无法直接看见的，而是通过迭代器对容器中的数据进行操作的；

## 28-29. 迭代器的分类对算法的影响

![](pics/Pasted%20image%2020230901103207.png)

- STL 中的迭代器有 5 种类型：
	- input iterator 只读迭代器，这种迭代器指向的对象不允许外界改变；
	- output iterator 只写迭代器；
	- forward iterator 允许写入型算法在这种迭代器的区间上进行读写操作，这种迭代器是单向的；
	- bidirectional iterator 允许双向移动；
	- random access iterator 不仅能双向移动，还可以进行计算如 iter + n, iter - n，即可以随机移动到任意位置；
- 这 5 种迭代器在 STL 中用的是类，同时也包含了继承关系，这样做法在后面能看到其用途；
- 前面介绍的容器中的迭代器类型：
	- array、vector 和 deque 都是 random access iterator，它们都可以随机移动；
	- list 是 bidirectional iterator 可以双向移动；
	- forward list 是 forward iterator 只允许单向移动；
	- set/map 等红黑树容器是双向的；
	- unordered 容器是单向的；

![](pics/Pasted%20image%2020230901104018.png)

- 这里测试了各种容器迭代器的具体类型，可以看到和前面介绍的是一样的；
- 这里也用到了 trait 技巧，通过萃取出迭代器的类型，会自动调用对应的打印函数，这也是为什么需要使用不同的类作为标签，而不是用整数等；
- 这里有两个特殊的迭代器，istream_iterator 和 ostream_iterator 它们分别是 input_iterator 和 output_iterator；

![](pics/Pasted%20image%2020230905100854.png)

- 迭代器的类型会决定算法具体的实现，这里以 distance 算法为例，这个算法是用来计算两个迭代器之间的距离的；
- distance 函数先通过 iterator_traits 萃取出迭代器的类型，然后创建了一个临时对象，这个临时对象的类型会决定最终调用哪个版本的函数；
- 如果迭代器类型是 input_iterator_tag 的话，会使用循环编译计算得到距离，如果是 random_access_iterator_tag 的话，会直接使用减号计算迭代器之间的距离（因为这种类型的迭代器重载了 `operator-()` 操作符；
- 这里我们可以看到迭代器的类型是如何对函数产生影响的，会根据不同的类型调用相应的函数；

![](pics/Pasted%20image%2020230905101759.png)

- 这里和上页差不多，主函数 advance 用来移动迭代器的位置，3 个次函数分别代表不同类型做的相应的操作（这里不赘述了）；
- 注意这里用了一个函数 iterator_category() 来得到迭代器的类型，本质和上页都是一样的，通过 iterator_trait 萃取出迭代器的类型；
- 另外注意实际上迭代器有 4 种类型，在右边的继承关系图上，但是这里只有 3 个次函数，因为迭代器的类型是用类表示的，继承关系可以看成 is a 的关系，所以对于 farword_iterator_tag 类型，虽然没有它的次函数，但是它是 input_iterator_tag 类型，所以会调用 input_iterator_tag 对应的次函数；

![](pics/Pasted%20image%2020230905102916.png)

- 这里是另外一个例子，copy 算法是将迭代器的内容拷贝到另外一个迭代器开始的位置上；
- copy 函数先根据迭代器类型是不是特化版本的 const char* 或 const wchar_t*，这两个版本可以直接用底层的 C 函数 memmove() 来进行移动数据；这里的特化是通过函数模版重载来进行的；
- 如果传入的是 InputIterator 迭代器类型又会进一步判断：
	- 如果迭代器类型是特化的 T* 和 const T* 的话，会通过 type trait 来萃取出 type 的类型是否为 trivial 的：
		- 如果有 trivial 的拷贝赋值操作符的话会调用 memmove() 函数；
		- 如果不是 trivial 最后会根据 n 来计算 for 循环的次数；
		- 这里的 trivial 指的是这个类型的拷贝赋值操作符是否重要，比如对于一个只有基本数据类型的类来说，它的拷贝构造/赋值，析构等函数都不用自定义，默认编译器提供的即可，这种类型就是 trivial 的；
	- 如果迭代器是 random access iterator 的话，可以通过计算迭代器的距离来计算循环的次数 n，最后也是调用 `_copy_d()` 函数；
	- 如果迭代器类型是 input iterator 的话，就通过比较的方式来进行循环；

![](pics/Pasted%20image%2020230905104421.png)

- 这里 unique_copy 不会拷贝重复的元素，这里要区分的是 OutputIterator 迭代器的类型，因为对于 output iterator 的话它是只写的，不可以进行读操作；
- 但是 forword iterator 就可以进行读操作，所以两者函数具体实现的时候有所差异；

## 30. 算法源码剖析（11 个例子）

![](pics/Pasted%20image%2020230906151500.png)

- 这里对比了 C 函数和 STL 提供的算法，STL 的算法是通过迭代器来进行操作的，C 函数中的算法并不是；

![](pics/Pasted%20image%2020230906152037.png)
- accumulate 算法的作用是将迭代器 first 到 last 中的元素进行累计操作；
- 有两个函数版本：
	- 第一个版本，传 3 个参数，分别是起始和结束的迭代器，init 是初始值；
	- 第二个版本，传 4 个参数，再上面的基础上，binary_op 是一个可以进行函数调用的对象；
- 右边是 4 个测试用例：
	- 第一个是默认 3 个参数的累加操作，计算结果为 160；
	- 第二个是传入了标准库的 `minus<int>()` 仿函数，这个函数作用是将两个元素进行相减，最后结果为 100 - 10 - 20 - 30 = 40；
	- 第三个是传入了自定义的函数指针，计算结果为 100 + 10 * 2 + 20 * 2 + 30 * 2 = 220；
	- 最后一个时传入了自定义的函数对象，计算结果为 100 + 10 * 3 + 20 * 3 + 30 * 3 = 280；
- C++ 中可以通过小括号进行调用的有以下几种情况：
	- 函数以及函数指针，`int f(int, int);`
	- 函数对象，即重载了 `operator()()` 的对象；
	- C++11 的 Lambda 表达式（本质上也是一个函数对象）；
	- std::function，它是可调用对象的包装器；

![](pics/Pasted%20image%2020230906153134.png)

- for_each 算法是遍历迭代器区间内的所有元素，然后通过函数 f 对元素进行操作；
- C++11 可以通过 for range 来遍历容器中的所有对象，而不需要通过复杂的迭代器的写法；
- {2, 3, 5, ...} 这是 C++11 新的规则，大括号会被编译器自动转为 initializer_list 容器；

![](pics/Pasted%20image%2020230906153430.png)
- replace 算法是将迭代器区间中所有等于旧值的元素，替换为新的值；
- replace_if 算法是根据谓词 pred 来将所有符合条件的旧值，替换为新值；
- replace_copy 算法是将所有的值放到新的区间中，对于等于旧值的元素要替换为新值；
- 从这里可以看出标准库的命名方式，一般带有 if 后缀的就是算法要满足一定的条件，而 copy 后缀的是将元素拷贝到新的区间；

![](pics/Pasted%20image%2020230906153950.png)

- count 算法是统计迭代器区间中等于 value 的元素个数，count_if 是通过符合谓词 pred 来进行计数；
- 关联式容器本身带有成员函数 count()，而其他的容器没有，一般推荐使用容器本身的成员函数，重复容器本身没有再考虑使用标准库的算法；

![](pics/Pasted%20image%2020230906155102.png)

- find 函数就是通过遍历来找到等于 value 的迭代器，find 效率比较低，关联式容器中有相应的成员函数 find，效率会高很多；

![](pics/Pasted%20image%2020230906155549.png)

- 这里并没有给出 sort 的源代码，STL 中大概是数据量比较大的时候，采用 Quick Sort 分段递归排序，如果分段的数据量小于 16 的时候，就采用 Insertion Sort，避免大量的递归导致栈溢出。如果递归层次过深，会采用 Heap Sort。
- 这里演示 sort 的用法，sort 传入起始和结束的迭代器，然后可以选择是否传入一个可调用对象，即自定义排序；
- 这里注意最下面的 rbegin() 和 rend()，它们是反向迭代器，这样的话相当于是将数组元素从大到小排序（默认的 begin() 到 end() 是从小到大排序）；
- 另外注意 list 和 forward_list 中有成员函数 sort，因为对于 STL 的 sort 来说需要是 random access iterator 类型的迭代器；

![](pics/Pasted%20image%2020230906160714.png)

- 这里来看下 reverse iterator 反向迭代器，对于 begin 来说返回的是第一个元素，而 rend() 返回的第一个元素的前一个元素（可以认为是个假象的元素）；
- end 返回的是最后一个元素的下一个位置，而 rbegin 返回的是最后一个元素；
- 所以反向迭代器和正向迭代器之间并不是完全对应的，需要通过适配器进行包装一下；

![](pics/Pasted%20image%2020230906161805.png)

- binary_search 算法是二分查找，来找到迭代器区间中是否有 val 值，它实际调用的 lower_bound 算法；
- lower_bound 算法返回的是大于等于给定值 val 的元素位置；
- upper_bound 算法返回的是大于给定值 val 的元素位置；
- 注意：进行二分查找的元素必须是有序的，否则结果不正确；

## 31. 仿函数和函数对象

![](pics/Pasted%20image%2020230908092223.png)

- 仿函数又称函数对象，它的作用是像函数一样可以通过小括号进行调用，本质上是重载了 `operator()` 的类；
- 这页展示了标准库的三种仿函数：算术类、逻辑运算类和相对关系类；

![](pics/Pasted%20image%2020230908092641.png)

- G2.9 中有几个非标准的仿函数，前面也接触过了，identity 是传入什么返回一模一样的值，select1st 和 select2nd 分别取出 pair 的第一个和第二个对象；
- G4.9 这几个函数名字变了，侯捷老师不赞同这种做法，虽然是非标准的，但是函数名字变化可能会对后续使用者代码产生影响；

![](pics/Pasted%20image%2020230908093844.png)

- 前面介绍标准库的仿函数的时候，会发现他们都继承了一个父类，标准库中有两种情况， unary_function 指的这个仿函数作用在一个参数上，binary_function 是作用在两个参数上，即一个是一元操作符，一个是二元操作符。
- 仿函数继承这两个类的作用是为了符合可以适配（adaptable）的条件，通过源代码可以看到这两个类并没有任何数据类型，只是定义了一些 typedef，所以仿函数继承他们的话也没有额外的开销。

## 32. 存在多种 Adapters

![](pics/Pasted%20image%2020230908095051.png)

有多种适配器（Adapters）：仿函数适配器（Functor Adapters）、迭代器适配器（Iterator Adapters）和容器适配器（Container Adapters）；

适配器的作用相当于是作为一个中间的桥梁，将一个类的接口转为另一个类的接口，这样可以使原本因接口不一致不能兼容的类，可以一起运作。

一般来说有两种方式让一个类包含另外一个类：一种是通过继承的方式，另外是通过内含的方式。这里是适配器都是采用内含的方式，适配器还需要像算法对迭代器提问那样，适配器需要对内含的对象进行提问，而内含的对象需要能够回答。这也是前面标准库的仿函数为什么需要继承 binary_function 和 unary_function 的原因。

![](pics/Pasted%20image%2020230908095144.png)

- 容器适配器在前面介绍容器的时候已经介绍了，这里简单回顾一下，容器适配器它们的内部有其他容器作为支撑，然后对相应的接口进行改造。

## 33. Binder2nd

![](pics/Pasted%20image%2020230908100727.png)

- 这页 PPT 细节很多，先从右上角 `count_if` 这里的作用是统计 vector 中大于等于 40 的个数，里面的 `not1` 和 `bind2nd` 都是函数适配器。
- `bind2nd(less<int>(), 40)` 它是将一个二元操作符的第二个参数绑定到一个具体的值，这里就是相当于是比较 x 是否小于 40；
- `bind2nd` 作为一个辅助函数，它实际是要返回一个 `binder2nd` 函数对象；
- `binder2nd` 是一个模版类，它也重载了 `operator()` ，所以 `binder2nd` 也是一个仿函数，可以看到这里函数适配器的原理：即将绑定的第二个参数作为一个值保存到 `binder2nd` 类中，然后在调用原来的仿函数。
- `binder2nd` 它的模版类型参数是 Operation，即其他函数对象的类型，这里可以看到为什么使用 `bind2nd` 作为一个辅助函数，因为可以利用函数模版类型推导，可以推导出传入的 `less<int>()` 这个类型，然后在创建一个 `binder2nd` 函数对象。
- `binder2nd` 另外的一个细节是用到了前面说的 typedef，通过 `Operation::second_argment_type` 来创建第二个参数类型的变量，`operator()` 返回值也用到了 `Operation::result_type` 类型，这些都是前面说的仿函数能成为可适配的条件。
- `binder2nd` 本身也可以作为继续可适配的对象，所以它也继承了 `unary_function` 类；
- 另外在使用 `Operation::second_argment_type` 的时候，发现前面都加上了 typename 关键字，这个关键字的作用是让编译器知道知道 `Operation::second_argment_type` 这是一个类型，而不是一个变量，这样编译器就可以通过了。

## 34. not1

![](pics/Pasted%20image%2020230908102727.png)

- not1 函数适配器，它也是作为一个辅助函数，创建一个 `unary_negate` 函数对象；
- `unary_negate` 是将传入的运算结果取否，具体细节和前面的 `binder2nd` 很像，这里不赘述了；

## 35. bind

到了 C++11 前面的 bind2nd，binder2nd 以及另外两个 bind1st，binder2st 都改成了 bind，而 bind 适配器的作用就更加广泛了。

下面的例子是来自：[cplusplus.com/reference/functional/bind/](https://cplusplus.com/reference/functional/bind/)

std::bind 可以绑定以下内容：
1. functions
2. function objects
3. member functions, \_1 必须是某个 object 地址
4. data members, \_1 必须是某个 object 地址

返回一个函数对象 ret，调用 ret 相当于调用上述 1,2,3，或者相当于取出 4。

bind 的参数可以绑定到一个值或占位符（_[placeholder](https://cplusplus.com/placeholders)_）
- 如果绑定到一个值，调用返回的函数对象将始终使用该值作为参数。
- 如果是占位符，则调用返回的函数对象将转发传递给调用的参数（其顺序号由占位符指定的参数）。

```cpp
// bind example
#include <iostream>     // std::cout
#include <functional>   // std::bind
using namespace std;

// a function: (also works with function object: std::divides<double> my_divide;)
double my_divide (double x, double y) {return x/y;}

struct MyPair {
  double a,b;
  double multiply() {return a*b;}
};

int main () {
  // 让占位符可见
  using namespace std::placeholders;    // adds visibility of _1, _2, _3,...

  // binding functions:
  // 绑定函数my_divide，参数是提供的10和2，所以调用这个函数会固定返回5
  auto fn_five = std::bind (my_divide,10,2);               // returns 10/2
  std::cout << fn_five() << '\n';                          // 5

  // 第一个参数为占位符，相当于fn_half有一个参数
  auto fn_half = std::bind (my_divide,_1,2);               // returns x/2
  std::cout << fn_half(10) << '\n';                        // 5
  
  // 两个参数都是占位符，但是顺序是反过来的，所以转发参数的结果是y/x
  auto fn_invert = std::bind (my_divide,_2,_1);            // returns y/x
  std::cout << fn_invert(10,2) << '\n';                    // 0.2

  // bind也可以提供一个参数类型，这个类型将作为返回值
  auto fn_rounding = std::bind<int> (my_divide,_1,_2);     // returns int(x/y)
  std::cout << fn_rounding(10,3) << '\n';                  // 3

  MyPair ten_two {10,2};

  // binding member functions:
  // 绑定成员函数，第一个参数必须是占位符，而且是一个object
  // 因为成员函数相当于是第一个参数是this指针
  auto bound_member_fn = std::bind (&MyPair::multiply,_1); // returns x.multiply()
  std::cout << bound_member_fn(ten_two) << '\n';           // 20

  // binding data members:
  // 绑定成员，这里绑定了ten_two的第一个成员
  auto bound_member_data = std::bind (&MyPair::a,ten_two); // returns ten_two.a
  std::cout << bound_member_data() << '\n';                // 10

  // 绑定成员，但是第一个参数传入的是object
  auto bound_member_data2 = std::bind (&MyPair::b, _1);     // returns x.b
  std::cout << bound_member_data2(ten_two) << '\n';        // 2

  vector<int> v{15, 37, 94, 50, 73, 58, 28, 98};
  int n = count_if(v.cbegin(), v.cend(), not1(bind2nd(less<int>(), 50)));
  std::cout << "n = " << n << std::endl;  // n = 5

  auto fn = bind(less<int>(), _1, 50);
  std::cout << count_if(v.cbegin(), v.cend(), fn) << std::endl; // 3
  std::cout << count_if(v.cbegin(), v.cend(), bind(less<int>(), _1, 50)) << std::endl; // 3
  return 0;
}
```

## 36. reverse_iterator

![](pics/Pasted%20image%2020230909104325.png)

- `reverse_iterator` 是逆向的迭代器，它和普通的 iterator 方向是相反的，可以通过 `rbegin()` 和 `rend()` 来获取逆向的迭代器，通过观察代码可以发现，其实就是对 `begin()` 和 `end()` 函数进行了简单的包装；
- 通过观察 `reverse_iterator` 代码可以看到它内部实际上有一个正向迭代器，它的 5 种 associated types 都是来自正向迭代器。
- `reverse_iterator` 前进的方向是从后往前，所以 `operator++` 就是让正向迭代器后退，`operator--` 是让正向迭代器前进。
- 最关键的问题在于，逆向迭代器和正向迭代器他们取值的时候，并不是对应的，逆向迭代器的取值位置是正向迭代器退一格的位置。

## 37. inserter

![](pics/Pasted%20image%2020230909105627.png)

- 如果想将一个容器中的元素拷贝到另外一个容器中，可以用 copy 算法，它接收 3 个参数，分别为源容器的起始迭代器，以及目标容器的待插入位置的迭代器；
- 通过观看左上角可以看到，将数组中 7 个元素都拷贝到 vector 中了，这里的 vector 提前预留好了空间，否则可能会访问到非法地址。
- 有没有什么办法不预留空间，可以直接拷贝呢？这里使用了 `inserter` 函数
- 先看左下方的用法，创建了两个 list foo 和 bar，然后插入 5 个元素，接着获得 foo 的第三个位置的迭代器，然后调用 `copy(bar.begin(), bar.end(), inserter(foo,it))`，最终表现的结果是将 bar 中所有的元素都插入到 foo 第三个位置之后。
- `inserter` 是一个辅助函数，最终返回一个 `insert_iterator` 迭代器类型，这里的问题在于对于 copy 函数来说，它已经是固定的了，它的代码中用了 `*result = *first` 即赋值运算符，如何将这一行为改成插入操作呢？
- `insert_iterator` 它存放了底层容器的指针以及插入位置的迭代器，这里巧妙的运用了操作符重载，对赋值运算符进行重载，实际上使用了容器的成员函数 `insert` 来完成插入操作。

## 38. ostream_iterator

![](pics/Pasted%20image%2020230912084146.png)

- ostream 和 istream 并不是 STL 6 大部件中的，所以这里他们的适配器称为 X 适配器；
- 先看左边的 ostream_iterator 的用法，它传入一个 `std::cout`，以及一个分隔符 ", "，然后使用 copy 将 vector 中的元素拷贝到 ostream_iterator 中；
- 由于 copy 拷贝目的端需要是一个迭代器，而且需要符合 copy 的代码，就要对 ostream_iterator 进行操作符重载；
- ostream_iterator 它包含了一个标准的输出流 (basic_ostream) 的指针 out_stream，以及一个分隔符 delim，它的 `operator*` 以及前 ++ 后 ++ 操作符重载返回的都是它自己，所以 copy 中 \*result 返回的还是 result；
- 让 copy 算法成功运行的关键是 `operator=` 赋值运算符，它将 value 打印到 \*out_stream 中，然后判断分隔符是否为空，不为空再打印分隔符。

## 39. istream_iterator

![](pics/Pasted%20image%2020230912085630.png)

- istream_iterator 它的作用是从输入流当中读取数据，这里 eos 没有绑定任何对象， 它作为流的末尾，iit 绑定了 `std::cin`；
- istream_iterator 使用 `operator*` 的时候，相当于是将迭代器中存储的值 value 返回了，而 `operator++` 通过 `*in_stream` 读取一个新的值；
- istream_iterator 在创建的时候就会读取一个值，可以看下构造函数 `istream_iterator(istream_type&s)`，它调用了 `++*this`。

![](pics/Pasted%20image%2020230912090715.png)

- 这个示例比较综合，它也是创建了 istream_iterator 绑定到 cin 以及一个空的 istream_iterator；
- copy 函数的起始和终止迭代器分别为 itt 和 eos，插入的位置是一个 inserter_iterator；
- copy 函数这里的意思是先从标准输入当中读入元素，然后插入到容器中，为了实现这个目的，需要看下 istream_iterator 相关的操作符重载；
- 红色标注的是 cin 关联的 istream_iterator，这里使用 `*first` 相当于将迭代器中存放的值 value 返回了，需要注意的是当创建 istream_iterator 的时候就会从 cin 当中读取一个元素了，下面的 `++first` 则继续从 cin 中读取元素，所以这里整体表现为不断的从标准输入 cin 中读取元素。最后因为 inserter 的作用，返回的是 inserter_iterator 就将元素都插入到容器中了。

## 40. 一个万用的 Hash Function

![](pics/Pasted%20image%2020230913091927.png)

- 如果想在 unordered 容器中使用自定义类的话，需要有计算自定义类的 hash 函数，这里有两种写法，一种是以函数对象形式，一种是以函数指针形式，函数指针传入到 unordered 容器的时候，需要写出类型以及函数地址，相对麻烦点。
- hash 函数的一个关键是如何计算 hash 值？

![](pics/Pasted%20image%2020230913092143.png)

- 如果只是将对象的所有成员变量取出来，然后通过 `std::hash` 函数计算累加，这种计算方式比较 naive；
- 一个更加通用的 hash 函数时右边的做法，这里用到了可变模板参数（variadic templates），会比较复杂，可变模板参数可以看下侯捷的 C++11 课程：
	- 首先 CustomerHash 的 `operator()` 运算符调用了 hash_val 函数，它实际调用的是 1，1 这里是可以接收任意个模版参数，然后它设置了一个种子 seed，继续调用 `hash_val(seed, args...)` ，此时会调用函数 2；
	- 函数 2 将 seed 和 val 通过 `hash_combine` 组合了一下，然后继续调用 `hash_val(seed, args...)` ，此时还是调用它自己，但是这里因为每次都会取出一个 val，所以函数模版参数的个数是在不断的减小的，最终调用函数 3，它也是将 seed 和 val 组合了一下；
	- 从上面的过程来看，相当于是不断的将每个值取出来，用 `hash_combine` 计算出整个哈希值。
- 这里的 `hash_combine` 为什么这样写，细节就不清楚了，但是 0x9e3779b9 这个值是怎么来的？

![](pics/Pasted%20image%2020230913093225.png)

- 0x9e3779b9 这个数借用了黄金比例，它的小数部分就是 9e3779b9

![](pics/Pasted%20image%2020230913093803.png)

- 除了前面说的两种方法，来在 unordered 容器中使用自定义类型，还有第 3 种方法，就是以 struct hash 偏特化形式表示 Hash Function；
- 标准库中 unordered 容器，它的 Hash 类型默认是以 `hash<T>` 类型，可以通过偏特化的方式自定义 `hash<T>`；

![](pics/Pasted%20image%2020230913094025.png)

- 这里呈现了自定义一个 MyString 类，然后在 std 命名空间中写一个偏特化版本的 `hash<MyString>` 类；
- 标准库中关于 string 类型的 hash 函数，也是这种方式定义的；

## 41. Tuple

![](pics/Pasted%20image%2020230914091131.png)

- tuple 可以将任意个任意类型的元素组合在一起，基本用法是 `tuple<T1, T2...>` 相当于创建了 N 个元素的结构体；
- 可以通过 `get<0>(t1)` 来获取 tuple 的第一个元素，`get<1>(t1)` 获得第二个元素，以此类推，通过 `get<idx>` 获取的元素返回的是左值，可以对其进行赋值；
- 创建一个 tuple 的时候，也可以直接通过 `make_tuple` 和 `auto` 来声明，这样可以自动进行类型推导，而不用写复杂的类型参数了；
- tuple 之间也可以进行比较和拷贝，比较的话需要保持元素**数量相同，类型可以比较**，如相同类型，或可以相互转换类型 (int&double)，比较的时候会一个接着一个进行比较；

![](pics/Pasted%20image%2020230914092354.png)

- tuple 它的实现很巧妙，利用了可变模版参数，声明的模版参数中 `typename... Tail` 长度是任意个；
- tuple 有一个成员变量 `Head m_head`，它还继承了 `tuple<Tail...>` ，相当于是每次都继承比自己少一个元素的 tuple，可以看下右边的例子：`tuple<int, float, string> t(41, 6.3 "nico");` ，对于 `tuple<int, float, string>` 它继承自 `tuple<float, string>`，每个类型中还有一个 m_head 变量，以此类推。
- tuple 这种继承关系，最终要止于 0 个参数的 `template<> class tuple<> {};`，相当于是递归的终点；

## 42. type traits

![](pics/Pasted%20image%2020230914094311.png)

- 在 G2.9 版本中，有一个 type traits，它的作用和 iterator traits 类似，是将模版类型萃取出一些 typedef，这里包括的：
	- has_trivial_default_constructor 表示构造函数是否重要；
	- has_trivial_copy_constructor 表示拷贝构造函数是否重要；
	- has_trivial_assignment_operator 表示拷贝赋值运算符是否重要；
	- has_trivial_destructor 表示析构函数是否重要；
	- is_POD_type 表示是否为 Plain Old Data 类型；
	- 这里的重要，指的是这些函数不执行任何额外操作，由编译器提供的即可，例如一个复数类来说，它的拷贝构造和析构函数就是不必要的，使用编译器自带的即可。
	- POD 类型是一种只包含普通数据成员（如整数、浮点数、指针等）而没有用户定义的构造函数、析构函数、复制构造函数或赋值运算符重载的 C++ 结构。这意味着它们在内存中具有紧凑的布局，可以直接通过内存复制来复制它们的实例，而不会引发复杂的对象生命周期管理。（from gpt）
- 上面这些类型是用 typedef 定义的，有两个结构体 `__ture_type` 和 `__false_type` 它们指的是 true or false，自定义这两个结构体的好处编译器可以通过不同的类型进行区分。
- 如果用户想自定义类，可以自行决定 type traits 中这些参数；

![](pics/Pasted%20image%2020230914101909.png)  
![](pics/Pasted%20image%2020230914101920.png)

- C++11 之后有大量的 type traits ，这里就不一一叙述了，可以查看手册 [cplusplus.com/reference/type_traits/](https://cplusplus.com/reference/type_traits/)
- 相关的测试用例也比较多，这里就跳过了，下面看两个简单的实现；

![](pics/Pasted%20image%2020230914102318.png)

- is_void 结构体用来判断传入的类型是否为 void 类型，这里可以看到它将传入的模版参数，放到 `__is_void_helper` 当中进行处理，它通过 `remove_cv<_TP>` 来去除 `_TP` 类型的 const 和 volatile 限定符；
- remove_volatile 和 remove_const 它们的实现是有一个泛化的模版类，以及一个特化的带 volatile 和 const 参数的模版类，这样的话如何是 const 或者 volatile 的就会进入特化版本的，此时的 type 就是普通的不带 cv 限定符的类型；
- `__is_void_helper` 它也是有一个泛化的和特化的版本，泛化版本继承的是 false_type，而特化版本继承自 true_type ，这样就可以用来区分是否为 void 类型了；

![](pics/Pasted%20image%2020230914103545.png)

- is_integral 和前面的 is_void 实现类型，想将模版参数的 cv 限定符去除，然后利用 `__is_integral_helper` 来找到对应的特化或者泛化版本，泛化版本继承了 false_type 即表示不是整数类型，而其他的特化版本，诸如 bool、char、signed char 等等都继承了 true_type，表示是整数类型；

## 44. cout

![](pics/Pasted%20image%2020230914104401.png)

- cout 平时也不陌生了，它本身是一个对象，可以将很多类型传入 cout 中，然后打印输出在终端上；
- cout 它是 `_IO_ostream_withassgin` 继承自 ostream 类，ostream 中有许多不同类型 `operator<<()` 操作符重载，如果想打印一些对象到终端上，只要 ostream 中有相应的重载即可，但是如果是自定义类型的话，可以考虑下面这种方式：

![](pics/Pasted%20image%2020230914104945.png)

- 如果自定义类型想通过 cout 打印到终端的话，需要像标准库这样定义一个全局的 `operator<<()` 函数重载；

最后的 move 相关的内容就跳过了，C++11 那门课程更加详细点。