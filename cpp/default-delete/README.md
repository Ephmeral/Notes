---
title: "08 - delete & default 关键字 1"
date: 2023-08-03T20:48:16+08:00
lastmod: 2023-08-03T20:48:23+08:00
tags: []
categories: []
cards-deck: CSNotes
image-auto-upload: true
from: 
---

# C++ 笔记 | 显示默认和删除函数
## 1. 特殊成员函数的生成

对于一个类来说，如果没有显示定义某些成员函数，但是实际上使用到了这些函数，编译器会自动生成这些成员函数，这些是类的**特殊成员函数**

C++98 的时候有 4 种，默认构造函数，析构函数，拷贝构造函数，拷贝赋值运算符。到了 C++11 新增了 2 个，即移动构造函数和移动赋值运算符。

下面是特殊成员函数自动生成的一些规则：
- **默认构造函数，仅仅当没有任何其他的构造函数，才会自动生成**；
- **如果显示的为类声明了拷贝构造/赋值运算符，编译器就不会自动生成拷贝操作**；
- **同上，如果显示的为类声明了移动构造/赋值运算符，编译器就不会自动生成**；
- **两个拷贝操作是相互独立的，声明一个不会限制编译器生成另一个**。如果声明一个拷贝构造函数，但是没有声明拷贝赋值运算符，而代码中用到了拷贝赋值，编译器会自动生成拷贝赋值运算符；
- **两个移动操作不是相互独立的**，声明了其中一个，另外一个编译器也不会在自动生成；
- **如果一个类显式声明了拷贝操作，编译器就不会生成移动操作**；编译器会认为平常拷贝对象的方法（逐成员拷贝）不适用于该类，逐成员移动也可能不适合；
- 反之，**如果一个类声明了移动操作，编译器就不会生成拷贝操作**，但实际并非如此，C++98 没有移动操作；

## 2. Rule of Zero/Three/Five
### Rule of Zero

如果一个类没有显示管理资源，不应该定义任何的特殊成员函数（默认构造函数、拷贝构造、拷贝赋值、移动构造、移动赋值），编译器会在需要的时候自动生成这些函数。

```cpp
struct MyResource {
    std::string name;
    int value;
};
```

上面定义了一个简单的结构体，它不需要管理任何资源，所以不需要为其定义任何的特殊成员函数，编译器会在需要的时候自动生成这些函数；

### Rule of Three

Rule of Three 告诉我们，如果声明了拷贝构造函数，拷贝赋值运算符，或者析构函数三者之一，也应该声明其余两个；

用户接管拷贝操作的需求几乎都是因为该类会做其他资源的管理，这也几乎意味着：
- （1）无论哪种资源管理如果在一个拷贝操作内完成，也应该在另一个拷贝操作内完成；
- （2）类的析构函数也需要参与资源的管理（通常是释放）；

通常要管理的资源是内存，比如类中有一个指针：

```cpp
class MyResource {
public:
    // 默认构造函数和析构函数
    MyResource() : data(new int[100]) {} 
    ~MyResource() { delete[] data; } 

    // 拷贝构造函数
    MyResource(const MyResource& other) : data(new int[100]) {
        std::copy(other.data, other.data + 100, data);
    }

    // 拷贝赋值运算符
    MyResource& operator=(const MyResource& other) {
        if (&other == this) { return *this; }
        std::copy(other.data, other.data + 100, data);
        return *this;
    }

private:
    int* data;
};
```

### Rule of Five

Rule of Five 是在 Rule of Three 上加上了移动构造函数和移动赋值运算符，它告诉我们，如果定义了拷贝构造、拷贝赋值、移动构造、移动赋值、析构函数中的一个，也应该定义剩下的特殊成员函数；

因为拷贝操作会阻止编译器自动生成移动操作，所以仅当下面条件成立时才会生成移动操作（当需要时）：
- 类中没有拷贝操作
- 类中没有移动操作
- 类中没有用户定义的析构

```cpp
class MyResource {
public:
    // 默认构造和析构函数
    MyResource() : data(new int[100]) {}
    ~MyResource() { delete[] data; }

    // 拷贝构造
    MyResource(const MyResource& other) : data(new int[100]) {
        std::copy(other.data, other.data + 100, data);
    }

    // 拷贝赋值运算符
    MyResource& operator=(const MyResource& other) {
        if (&other == this) { return *this; }
        std::copy(other.data, other.data + 100, data);
        return *this;
    }

    // 移动构造
    MyResource(MyResource&& other) noexcept : data(other.data) {
        other.data = nullptr;
    }

    // 移动赋值运算符
    MyResource& operator=(MyResource&& other) noexcept {
        if (&other == this) { return *this; }
        delete[] data;
        data = other.data;
        other.data = nullptr;
        return *this;
    }

private:
    int* data;
};
```

### 小结

C++11 对于特殊成员函数处理的规则如下：

- **默认构造函数**：和 C++98 相同，仅当类不存在用户声明的构造函数时才自动生成。
- **析构函数**：基本上和 C++98 相同；稍微不同的是现在析构默认 `noexcept`。和 C++98 一样，仅当基类析构为虚函数时该类析构才为虚函数。
- **拷贝构造函数**：和 C++98 运行时行为一样：逐成员拷贝 non-static 数据。仅当类没有用户定义的拷贝构造时才生成。如果类声明了移动操作它就是 _delete_ 的。
- **拷贝赋值运算符**：和 C++98 运行时行为一样：逐成员拷贝赋值 non-static 数据。仅当类没有用户定义的拷贝赋值时才生成。如果类声明了移动操作它就是 _delete_ 的。
- **移动构造函数**和**移动赋值运算符**：都对非 static 数据执行逐成员移动。仅当类没有用户定义的拷贝操作，移动操作或析构时才自动生成。

## 3. 显示默认

前面提到编译器会自动生成特殊成员函数，但是如果定义了某个特殊成员函数，可以会影响到其他特殊成员函数的自动生成，例如给类提供一个其他参数的构造函数，会阻止默认构造函数的生成：

```cpp
#include <string>
#include <vector>
class City {
  std::string name;
  std::vector<std::string> street_name;

public:
  City(const char *n) : name(n) {}
};

int main() {
  City a("wuhan");
  City b;  // 编译失败，自定义构造函数抑制了默认构造函数
  b = a;
}
```

为了使上面编译成功，需要自定义一个默认构造函数：

```cpp
class City {
  std::string name;
  std::vector<std::string> street_name;

 public:
  City(const char *n) : name(n) {}
  City() {}  // 新添加的构造函数
};
```

如此，虽然添加了这个默认构造函数，但是实际上并没有做任何操作；

另外一个例子：

```cpp
#include <iostream>
#include <type_traits>
using namespace std;

class Trivial {
  int i;

 public:
  Trivial(int n) : i(n), j(n) {}
  Trivial() {}
  int j;
};

int main() {
  Trivial a(5);
  Trivial b;
  b = a;
  std::cout << "std::is_trivial<Trivial>   : "
            << std::is_trivial<Trivial>() << std::endl;
}
/*
$ g++ is_not_trivial.cpp && ./a.out 
std::is_trivial<Trivial>   : 0
*/
```

构造函数 `Trivial(int n)`，它导致编译器抑制添加默认构造函数，于是 `Trivial` 类转变为非平凡类型。无参数的构造函数，也会让 `Trivial` 类转变为非平凡类型；

为了解决上述问题， C++11 标准提供了一种方法能够简单有效又精确地控制默认特殊成员函数的添加，这种方法称为显式默认。

显示默认只需要在声明的函数尾部添加 =default 即可，表示编译器添加特殊函数的默认版本：

```cpp
struct type {
  type() = default; // 类内部声明
  virtual ~type() {};
  type(const type &);
};
type::type(const type &) = default; // 类外部声明
```

显示默认可以在类内部声明，也可以在类外部声明；

由此可以解决上面转为非平凡类型的问题：

```cpp
#include <iostream>
#include <type_traits>
using namespace std;

class NonTrivial {
  int i;

 public:
  NonTrivial(int n) : i(n), j(n) {}
  NonTrivial() {}
  int j;
};

class Trivial {
  int i;

 public:
  Trivial(int n) : i(n), j(n) {}
  Trivial() = default;
  int j;
};

int main() {
  Trivial a(5);
  Trivial b;
  b = a;
  std::cout << "std::is_trivial<Trivial>   : " << std::is_trivial<Trivial>()
            << std::endl;
  std::cout << "std::is_trivial<NonTrivial> : " << std::is_trivial<NonTrivial>()
            << std::endl;
}
/*
$ ./a.out 
std::is_trivial<Trivial>   : 1
std::is_trivial<NonTrivial> : 0
*/
```

这里将构造函数 `NonTrivial() {}` 替换为显式默认构造函数 `Trivial() = default`，类就从非平凡类型恢复到平凡类型了。这样一来，既让编译器为类提供了默认构造函数，又保持了类本身的性质。

## 4. 显示删除

如果想禁止重载函数的某些版本，可以考虑将这个函数声明为 private：

```cpp
class Base {
  void foo(long &);

 public:
  void foo(int) {}
};

int main() {
  Base b;
  long l = 5;
  b.foo(8);
  b.foo(l);  // 编译错误，foo(long)为private
}
```

但是此时如果有个子类继承自 `Base`，且还想沿用基类 `Base` 的 `foo` 函数，这里使用 `using` 说明符将 `Base` 的 `foo` 成员函数引入子类，代码如下：

```cpp
class Base {
  void foo(long &);

 public:
  void foo(int) {}
};

class Derived : public Base {
 public:
  using Base::foo; // using这里就会报错，无法将private引入子类中
  void foo(const char *) {}
};

int main() {
    Derived d;
    d.foo("hello");
    d.foo(5);
}
```

这里看起来比较合理，但是 using 说明符无法将私有成员函数引入到子类中，会报错；

C++11 添加了和显示默认类似的语法，显示删除，将需要删除的函数声明的尾部添加 =delete 即可：

```cpp
struct type {
  type() = default;
  virtual ~type() = delete;
  type(const type &);
};
type::type(const type &) = default;
```

注意显示删除 =delete 关键字**只能放在函数内部**，而不能放在函数外部；

下面是用 =delete 来解决禁止重载函数的继承问题：

```cpp
class Base {
//     void foo(long &);
 public:
  void foo(int) {}
  void foo(long &) = delete;
};

class Derived : public Base {
 public:
  using Base::foo;
  void foo(const char *) {}
};

int main() {
    Derived d;
    d.foo("hello");
    d.foo(5);
}
```

此时不需要将 `foo(long)` 函数声明为 private，如果还是声明 private，这里还是会报错；

### 显示删除其他用法

显示删除还可以用来对普通函数使用，C++ 会和 C 语言一样，含糊的、能被视作数值的任何类型都能隐式转换为 int，但是有一些调用可能是没有意义的：

```cpp
if (isLucky('a')) … //字符 'a' 是幸运数？  
if (isLucky(true)) … //"true" 是?  
if (isLucky(3.5)) … //难道判断它的幸运之前还要先截尾成 3？
```

可以用显示删除，过滤某些类型：

```cpp
bool isLucky(int number);       //原始版本
bool isLucky(char) = delete;    //拒绝char
bool isLucky(bool) = delete;    //拒绝bool
bool isLucky(double) = delete;  //拒绝float和double

// 此时下面的调用会编译错误
if (isLucky('a')) …     //错误！调用deleted函数
if (isLucky(true)) …    //错误！
if (isLucky(3.5f)) …    //错误！
```

显示删除还可以用在禁止某些模版的实例化，如果有一个模版仅支持原始指针，但是有两个特例 void* 和 char* 通常需要禁止这两个版本，可以使用 =delete 标识：

```cpp
template<typename T>
void processPointer(T* ptr);

template<>
void processPointer<void>(void*) = delete;
template<>
void processPointer<char>(char*) = delete;
```

在一个类中如果想禁止函数模版的某些实例化，不能通过声明为 private 的方式来进行，因为**模板特例化必须位于一个命名空间作用域，而不是类作用域**；但是显示删除不会出现这个问题，因为它不需要一个不同的访问级别，且可以在类外被删除（因此位于命名空间作用域）：

```cpp
class Widget {
 public:
  template <typename T>
  void processPointer(T* ptr) {
    //…
  }
 // …
private:
/*
    template<>                          
    void processPointer<char>(char*); //编译错误！
*/
};

template <>
void Widget::processPointer<void>(void*) = delete;  // 还是public，但是已经被删除了
```

另外显示删除可以用于类的 `new` 运算符和析构函数。删除类的 new 运算符，会阻止该类在堆上创建对象：

```cpp
struct type {
  void *operator new(std::size_t) = delete;
};

type global_var;
int main() {
  static type static_var;
  type auto_var;
  type *var_ptr = new type;  // 编译失败，该类的new已被删除
}
```

将析构函数显示删除会阻止类的析构，像自动变量、静态变量或者全局变量这种会隐式调用析构函数的对象就无法创建了，通过 `new` 运算符创建的对象也无法通过 `delete` 销毁：

```cpp
struct type {
  ~type() = delete;
};
type global_var;             // 编译失败，析构函数被删除无法隐式调用

int main() {
  static type static_var;    // 编译失败，析构函数被删除无法隐式调用
  type auto_var;             // 编译失败，析构函数被删除无法隐式调用
  type *var_ptr = new type;
  delete var_ptr;            // 编译失败，析构函数被删除无法显式调用
}
```

只有 new 创建的对象会成功，但是不能被销毁，可以用来实现单例模式；

## 参考资料

- 《Effective Modern C++》条款十一、十七
- 《现代 C++ 语言核心特性解析》 第 10 章