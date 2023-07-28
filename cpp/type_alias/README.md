## 类型别名

对于一个名字比较长的类型，可以利用 typedef 起个别名：

```cpp
typedef std::map<int, std::string>::const_iterator map_const_iter;
map_const_iter iter;
```

C++11 之后可以利用 using 起别名：

```cpp
using map_const_iter = std::map<int, std::string>::const_iterator;
map_const_iter iter;
```

以上来说两者几乎一致（除了函数定义外）：

```cpp
typedef void(*func1)(int, int);
using func2 = void(*)(int, int);
```

typedef 在函数类型定义了其他类型定义有点区别，而 using 则是统一的；

## 模版别名

using 的强大之处在于可以为模版起别名（又称别名模版 alias templates），考虑给一个自定义内存分配器的链表起个别名：

```cpp
template<typename T>
using MyAllocList = std::list<T, MyAlloc<T>>; 

MyAllocList<Widget> lw; 
```

如果使用 typedef 需要重新定义一个 MyAllocList 结构体：

```cpp
template<typename T>
struct MyAllocList {
	typedef std::list<T, MyAlloc<T>> type;
};

MyAllocList<Widget>::type lw;
```

如果想在一个模版内使用 typedef 声明一个链表对象，这个对象又使用了模版形参，就必须在 typedef 前加上 typename 关键字：

```cpp
template<typename T>
class Widge {
private:
	typename MyAllocList<T>::type list;
};
```

而 using 则没有那么麻烦：

```cpp
template<typename T>
using MyAllocList = std::list<T, Alloc<T>>;

template<typename T>
class Widge {
private:
	MyAllocList<T> list; // 没有typename，没有::type
}
```

## 参考资料
- 《Effective Modern C++》条款九
- 《现代 C++ 语言核心特性解析》 第 22 章