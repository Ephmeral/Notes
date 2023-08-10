# 常量表达式
## constexpr 值

const 对象是运行时常量，在一些场景使用的时候会无法编译（例如 case，定义数组大小，定义枚举成员等），这几个场景可以用宏来解决，但是现代 C++ 建议尽量减少使用宏，因为宏只是对字符简单替换，而没有类型检查等，是不安全的；

所以常量表达式是为了解决上面问题出现的，在变量或函数前用 `constexpr` 修饰即可代码常量表达式（对于函数有些额外的限制）；

下面代码中 index0 是一个 const 变量，它可能是一个运行期变量（调用其他函数得到值），下面的代码是无法通过编译的：

```cpp
// const_val. cpp
#include <iostream>
#include <tuple>
using namespace std;

int get_index0() {
    return 0;
}

const int index0 = get_index0();
#define index1 1

const int x_size = 5 + 8;
#define y_size 6 + 7
// 数组长度
char buffer[x_size][y_size] = {0};

// 枚举成员
enum {
  enum_index0 = index0,
  enum_index1 = index1,
};

int main(int argc, char **argv) {
    // case语句
    switch (argc) {
    case index0:
        std::cout << "index0" << std::endl;
        break;
    case index1:
        std::cout << "index1" << std::endl;
        break;
    default:
        std::cout << "none" << std::endl;
    }

    std::tuple<int, char> tp = std::make_tuple(4, '3');
    // 非类型的模板参数
    int x1 = std::get<index0>(tp);
    char x2 = std::get<index1>(tp);
}
```

如果在 `get_index0` 函数返回类型前加上 `constexpr` 关键字，上面的代码则可以正常运行，此时 `get_index0` 是一个**常量表达式函数**，下文会介绍；

对于一个常量表达式来说，只需要在声明变量的前面加上 `constexpr` 关键字，下面是简单的了 `const` 进行对比：

```cpp
// constexpr_val. cpp
#include <iostream>
using namespace std;

int main() {
    // 使用consexpr定义变量x
    constexpr int x = 43;
    char buf[x] = {0};

    // 使用const定义z，下面的代码可能会编译不通过
    int y = 43;
    const int z = y;
    char buf2[z] = {0};    
}
```

`constexpr` 是一个加强版的 `const`，它不仅要求常量表达式是常量，并且要求是一个编译阶段就能够确定其值的常量；

## constexpr 函数

C++11 对 constexpr 函数要求的规则如下：

1. 函数必须返回一个值，所以它的返回值类型不能是 void；
2. 函数体必须只有一条语句：return expr，其中 expr 必须也是一个常量表达式。如果函数有形参，则将形参替换到 expr 中后，expr 仍然必须是一个常量表达式；
3. 函数使用之前必须有定义；
4. 函数必须用 constexpr 声明；

下面是几个例子：

```cpp
constexpr int max_unsigned_char() { 
    return 0xff; 
}

constexpr int square(int x) { 
    return x * x; 
}

constexpr int abs(int x) { 
    return x > 0 ? x : -x; 
}
```

下面是几个反例：

```cpp
// 下面的几个函数使用g++ constexpr_func.cpp -std=c++11 则无法通过编译
// 返回类型为void
constexpr void foo() {} 

// ++x不是常量表达式 
constexpr int next(int x) { return ++x; }

int g() { return 42; }

// g() 不是常量表达式
constexpr int f() { return g(); }
// 只有声明没有定义
constexpr int max_unsigned_char2();

enum {
  max_uchar = max_unsigned_char2()
}

// 多条语句
constexpr int abs2(int x) {
  if (x > 0) {
    return x;
  } else {
    return -x;
  }
}

// 多条语句
constexpr int sum(int x) {
  int result = 0;
  while (x > 0) {
    result += x--;
  }
  return result;
}
```

上面几个反例中都是不符合 C++11 规则的，但是对于最后一个 sum 函数可以使用递归，能够正常编译：

```cpp
constexpr int sum(int x) {
  return x > 0 ? x + sum(x - 1) : 0; // sum(x-1)本身也是常量表达式，三目运算符只是一条语句
}
```

## constexpr 构造函数

`constexpr` 可以支持用户自定义类型：

```cpp
struct X {
    int x1;
};

int main() {
    constexpr X x = {100};
    char buf[x.x1] = {0};
}
```

如果想为 X 添加构造函数，以及 `get` 函数的话，上面的代码则不能正常运行，但是 `constexpr` 也是可以支持构造函数的：

```cpp
class Y {
public:
    constexpr Y() : val(0) {}
    constexpr Y(int _val) : val(_val) {}
    constexpr int get() const { return val; }
private:
    int val;
};

int main() {
	constexpr Y y = {100};
    char buf2[y.get()] = {0};

    int i = 8;
    // constexpr X x(i);     // 编译失败，不能使用constexpr声明
    X y(i);               // 编译成功，退化为普通成员函数
}
```

上面代码中 `constexpr` 让构造函数成为常量表达式函数了，这样自定义的类似即为常量表达式了。另外只要类型的声明对象不能为常量表达式值，常量表达式构造函数可以退化为普通构造函数，见上面的变量 `i` 相关代码；

上面代码还有个地方注意的是：`get` 函数后面加上的 const 常量性在 C++11 中是多余的，因为 C++11 会自动将常量表达式函数添加上 const 属性，但是到了 C++14 的时候是没有这个特性的，所以上面代码如果将 `get` 函数后面的 const 去掉，再使用 `-std=c++14` 是无法编译通过的；

最后 `constexpr` 声明自定义类型的变量，必须确保这个自定义类型的析构函数是平凡的，否则也是无法通过编译的。平凡析构函数必须满足下面 3 个条件：

1. 自定义类型中不能有用户自定义的析构函数；
2. 析构函数不能是虚函数；
3. 基类和成员的析构函数必须都是平凡的；

## C++14 对 constexpr 的增强

C++14 标准对常量表达式函数的改进如下：

1. 函数体允许声明变量，除了没有初始化、static 和 thread_local 变量；  
2. 函数允许出现 if 和 switch 语句，不能使用 go 语句；  
3. 函数允许所有的循环语句，包括 for、while、do-while；  
4. 函数可以修改生命周期和常量表达式相同的对象；  
5. 函数的返回值可以声明为 void；  
6. constexpr 声明的成员函数不再具有 const 属性；

有了 C++14 的拓展，之前多条语句中未能通过编译的，现在可以成功编译了：

```cpp
// 多条语句
constexpr int abs2(int x) {
  if (x > 0) {
    return x;
  } else {
    return -x;
  }
}

// 多条语句
constexpr int sum(int x) {
  int result = 0;
  while (x > 0) {
    result += x--;
  }
  return result;
}

// 下面也能成功编译，因为规则4
constexpr int next(int x) { return ++x; }
```

构造函数常量表达式也得到了增强：

```cpp
#include <iostream>

class X {
 public:
  constexpr X() : x1(5) {}
  // 构造函数中可以有多条语句
  constexpr X(int i) : x1(0) {
    if (i > 0) {
      x1 = 5;
    } else {
      x1 = 8;
    }
  }
  constexpr void set(int i) { x1 = i; }
  constexpr int get() const { return x1; }

 private:
  int x1;
};

// 可以修改生命周期和常量表达式一样的变量
constexpr X make_x() {
  X x;
  x.set(42);
  return x;
}

int main() {
  constexpr X x1(-1);
  constexpr X x2 = make_x();
  constexpr int a1 = x1.get();
  constexpr int a2 = x2.get();
  std::cout << a1 << std::endl;  // 8
  std::cout << a2 << std::endl;  // 42
}
/*
$ g++ constexpr_ctor_c14.cpp -std=c++14 && ./a.out 
8
42
*/
```

#TODO C++17 lambda 的 constexpr，内联性，if constexpr

