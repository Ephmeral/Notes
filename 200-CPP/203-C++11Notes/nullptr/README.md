# 指针字面量 nullptr
## 零值字面量

C++ 标准中 0 即使一个整型常量，也可以是一个空指针常量，0 作为空指针常量的时候，可以隐式的转为各种指针类型，实际上标准库的 NULL 在 C++11 标准前就是 0，可以看下面的代码：

```cpp
// zero_to_pointer.cpp
#include <iostream>
#include <cstdio>
#include <cstdlib>
using namespace std;

int main() {
    int* x = 0;
    int* nx = NULL;
    char* c = 0;
    char* nc = NULL;

    printf("x = %p\n", x);
    printf("nx = %p\n", nx);
    printf("c = %p\n", c);
    printf("nc = %p\n", nc);

    return 0;
}
/*
$ g++ zero_to_pointer.cpp && ./a.out 
x = (nil)
nx = (nil)
c = (nil)
nc = (nil)
*/
```

0 和 NULL 打印的结果一样，可以相互替代，这里的 NULL 是一个宏，在 C++11 标准之前其本质就是 0（在 C 语言中定义为 (void*)0）：

```cpp
#ifndef NULL
    #ifdef __cplusplus
        #define NULL 0
    #else
        #define NULL ((void *)0)
    #endif
#endif
```

当对指针类型和整型类型进行重载时，这可能会导致二义性：

```cpp
// ambiguity.cpp
#include <iostream>
using namespace std;

void f(int x) {
    std::cout << "call f(int x)" << std::endl;
}

void f(bool x) {
    std::cout << "call f(bool x)" << std::endl;
}

void f(char* x) {
    std::cout << "call f(void* x)" << std::endl;
}

int main() {
    f(0);
    f(NULL); // 这里会编译错误，因为f(int)和f(void*)都可以调用
    f(reinterpret_cast<char *>(NULL)); // NULL转为char*类型，正常调用
}
```

上面代码在 gcc 中会编译失败，会出现二义性错误，如果将 NULL 强制转为 char* 类型便可正常调用 `f(char*)` 函数；

## nullptr 关键字

C++11 定义了 nullptr 关键字来表示空指针字面量：
- 它是一个 `std::nullptr_t` 类型的纯右值；
- nullptr 不允许用在算术表达式中，或与非指针类型进行比较（0 除外）；
- nullptr 可以隐式转为任何指针类型，无法转为非指针类型；
- 0 保留可以转为整型和空指针的能力；

```cpp
// nullptr.cpp
#include <iostream>
using namespace std;

struct Point {
    int x, y, z;
};

int main() {
    // nullptr可以赋值给任意类型的指针，表示空指针
    int *pi = nullptr;
    char *pc = nullptr;
    long *pl = nullptr;
    Point *p = nullptr;

    // nullptr不允许赋值给非指针类型
    // int x = nullptr; 

    // nullptr不允许用在算术表达式中
    // int x = 0 + nullptr; // 编译错误
    
    int y = 1;

    // nullptr不允许与任何非指针类型比较（0除外）
    // if (y == nullptr) {
    //     std::cout << "y是一个空指针" << std::endl;
    // }
    if (0 == nullptr) {
        std::cout << "0是一个空指针" << std::endl;
    }
}
```

对于 `std::nullptr_t` 类型，它本身是通过 decltype 定义得到的，下面是标准库的定义：

```cpp
namespace std
{
#if __cplusplus >= 201103L
  typedef decltype(nullptr)	nullptr_t;
#endif
}
```

`std::nullptr_t` 是一个类型，所以可以自定义 null 指针，但是与 nullptr 关键字是有区别的，nullptr 是纯右值，而自定义的 null 指针是个左值，可以取地址：

```cpp
// nullptr_t.cpp
#include <iostream>
#include <cassert>
using namespace std;

int main() {
  std::nullptr_t null1, null2;

  char* ch = null1;
  char* ch2 = null2;
  assert(ch == 0);
  assert(ch == nullptr);
  assert(ch == null2);
  assert(null1 == null2);
  assert(nullptr == null1);
    
  // null1和null2是左值，可以成功获取对象指针，并且指针指向的内存地址不同
  std::cout << "&null1 = " << &null1 << std::endl;  
  std::cout << "&null2 = " << &null2 << std::endl;
}
```

## nullptr 在函数重载和模版重载中的作用

前面提到使用 NULL 的时候，重载的函数模版可能有二义性，但是使用 nullptr 就不会有这个问题：

```cpp
// ambiguity.cpp
#include <iostream>
using namespace std;

void f(int x) { std::cout << "call f(int x)" << std::endl; }

void f(bool x) { std::cout << "call f(bool x)" << std::endl; }

void f(char* x) { std::cout << "call f(char* x)" << std::endl; }

int main() {
  f(0);                              // call f(int x)
  // f(NULL);                        // 编译失败
  f(reinterpret_cast<char*>(NULL));  // call f(char* x)
  f(nullptr);                        // call f(char* x)
}
```

可以为函数模板或者类设计一些空指针类型的特化版本，如下：

```cpp
//  specialize_template.cpp
#include <iostream>

template <class T>
struct widget {
  widget() { std::cout << "template" << std::endl; }
};

template <>
struct widget<std::nullptr_t> {
  widget() { std::cout << "nullptr" << std::endl; }
};

template <class T>
widget<T>* make_widget(T) {
  return new widget<T>();
}

int main() {
  auto w1 = make_widget(0);        // template
  auto w2 = make_widget(nullptr);  // nullptr
}
```

## 参考资料

- 《Effective Modern C++》条款八
- 《现代 C++ 语言核心特性解析》 第 23 章