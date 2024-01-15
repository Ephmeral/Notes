## 统一初始化

C++11 添加了 `{}` 来进行统一初始化，下面是几种初始化方式：

```cpp
int x(0);     // 使用小括号初始化
int y = 0;    // 使用"="初始化
int z{0};     // 使用花括号初始化
int z = {0};  // 使用"="和花括号，和花括号认为是等价的
```

几个好处：

1. **为非静态成员数据指定默认初始值**，C++11 也允许使用 = 来设定初始值：

```cpp
class Widget {
  // ...
 private:
  int x{0};   // 没问题，x初始值为0
  int y = 0;  // 也可以
  int z(0);   // 错误！
};
```

2. **避免隐式缩窄转换**（narrowing conversion）：如果一个使用了括号初始化的表达式的值，不能保证由被初始化的对象的类型来表示，代码就不会通过编译：

```cpp
double x, y, z;
int sum1{x + y + z};  // 错误！double的和可能不能表示为int
```

3. **防止 Most Vexing Parse**：C++ 规定任何能被决议为一个声明的东西必须被决议为声明。

```cpp
Widget w1(10);  // 使用实参10调用Widget的一个构造函数
Widget w2();    // Most Vexing Parse！声明一个函数w2，返回Widget对象
Widget w3{};    // 调用没有参数的构造函数构造对象
```

4. **列表初始化**：可以用大括号初始化类的成员变量。

## 列表初始化

### 列表初始化优先级

`std::vector` 允许指定容器初始大小以及一个值填充容器，也运行 `std::initializer_list` 构造函数：

```cpp
// 使用非std::initializer_list构造函数
// 创建一个包含10个元素的std::vector，
// 所有的元素的值都是20
std::vector<int> v1(10, 20);
// 使用std::initializer_list构造函数
// 创建包含两个元素的std::vector，
// 元素的值为10和20
std::vector<int> v2{10, 20};
```

如果有一个类同时拥有满足列表初始化的构造函数，且其中一个是以 `std::initializer_list` 为参数，那么编译器将优先以 `std::initializer_ list` 为参数构造函数。

### 无 std::initializer_list 形参构造函数

对于一个类来说，如果没有 `std::initializer_list` 形参的构造函数，使用花括号和小括号会产生相同的调用结果：

```cpp
// initializer_list.cpp
#include <iostream>
using namespace std;

class Widget {
 public:
  Widget(int i, bool b) { std::cout << "Widget(int, bool)" << std::endl; }
  Widget(int i, double d) { std::cout << "Widget(int, double)" << std::endl; }

private:
  int m_i;
  bool flag;
};

int main() {
  Widget w1(10, true);  // 调用第一个构造函数
  Widget w2{10, true};  // 也调用第一个构造函数
  Widget w3(10, 5.0);   // 调用第二个构造函数
  Widget w4{10, 5.0};   // 也调用第二个构造函数
}
/*
$ g++ initializer_list.cpp 
$ ./a.out 
Widget(int, bool)
Widget(int, bool)
Widget(int, double)
Widget(int, double)
*/
```

### 有 std::initializer_list 形参构造函数
如果有一个 `std::initializer_list<double>` 构造函数，上面的花括号就会调用使用 `std::initializer_list` 作为参数的构造函数：

```cpp
// initializer_list.cpp
#include <iostream>
using namespace std;

class Widget {
 public:
  Widget(int i, bool b) { std::cout << "Widget(int, bool)" << std::endl; }
  Widget(int i, double d) { std::cout << "Widget(int, double)" << std::endl; }
  Widget(std::initializer_list<double> il) {
    std::cout << "Widget(std::initializer_list<double>)" << std::endl;
  }

 private:
  int m_i;
  bool flag;
};

int main() {
  Widget w1(10, true);
  Widget w2{10, true};
  Widget w3(10, 5.0);
  Widget w4{10, 5.0};
}
/*
$ g++ initializer_list.cpp && ./a.out 
Widget(int, bool)
Widget(std::initializer_list<double>)
Widget(int, double)
Widget(std::initializer_list<double>)
*/
```

普通的构造函数和移动构造函数也会被 `std::initializer_list` 构造函数劫持：

```cpp
class Widget {
 public:
  Widget(int i, bool b);                          // 同之前一样
  Widget(int i, double d);                        // 同之前一样
  Widget(std::initializer_list<long double> il);  // 同之前一样
  operator float() const;                         // 转换为float
//   …
};

Widget w5(w4);  // 使用小括号，调用拷贝构造函数
Widget w6{w4};  // 使用花括号，调用std::initializer_list构造函数（w4转换为float，float转换为double）
Widget w7(std::move(w4));  // 使用小括号，调用移动构造函数
Widget w8{std::move(w4)};  // 使用花括号，调用std::initializer_list构造函数（与w6相同原因）
```

### 有 std::initializer_list 形参构造函数（但类型不匹配）

如果提供一个无法转换的类型，上述花括号就不会调用 `std::initializer_list` 构造函数：

```cpp
class Widget { 
public:  
    Widget(int i, bool b);                              //同之前一样
    Widget(int i, double d);                            //同之前一样
    //现在std::initializer_list元素类型为std::string
    Widget(std::initializer_list<std::string> il);
    …                                                   //没有隐式转换函数
};

Widget w1(10, true);     // 使用小括号初始化，调用第一个构造函数
Widget w2{10, true};     // 使用花括号初始化，现在调用第一个构造函数
Widget w3(10, 5.0);      // 使用小括号初始化，调用第二个构造函数
Widget w4{10, 5.0};      // 使用花括号初始化，现在调用第二个构造函数
```

### 花括号为空

空的花括号会调用默认构造函数，而不是空的 `std::initializer_list`，如果想调用空的 `std::initializer_list` 需要再加一层花括号：

```cpp
class Widget {
 public:
  Widget();                               // 默认构造函数
  Widget(std::initializer_list<int> il);  // std::initializer_list构造函数

  …  //没有隐式转换函数
};

Widget w1;      // 调用默认构造函数
Widget w2{};    // 也调用默认构造函数
Widget w3();    // Most Vexing Parse！声明一个函数
Widget w4({});  // 使用空花括号列表调用std::initializer_list构造函数
Widget w5{{}};  // 同上
```




## 补充：隐式缩窄转换

C++ 中隐式缩窄转换的 4 条规则：

1．从浮点类型转换整数类型；

2．从 `long double` 转换到 `double` 或 `float`，或从 `double` 转换到 `float`，除非转换源是常量表达式以及转换后的实际值在目标可以表示的值范围内；

3．从整数类型或非强枚举类型转换到浮点类型，除非转换源是常量表达式，转换后的实际值适合目标类型并且能够将生成目标类型的目标值转换回原始类型的原始值；

4．从整数类型或非强枚举类型转换到不能代表所有原始类型值的整数类型，除非源是一个常量表达式，其值在转换之后能够适合目标类型；

结合标准的例子就很容易理解了：

```cpp
int x = 999;
const int y = 999;
const int z = 99;
const double cdb = 99.9;
double db = 99.9;

char c1 = x;  // 编译成功，传统变量初始化支持隐式缩窄转换
char c2{ x }; // 编译失败，可能是隐式缩窄转换，对应规则4
char c3{ y }; // 编译失败，确定是隐式缩窄转换，999超出char能够适应的范围，对应规则4
char c4{ z }; // 编译成功，99在char能够适应的范围内，对应规则4
unsigned char uc1 = { 5 };  // 编译成功，5在unsigned char能够适应的范围内，对应规则4
unsigned char uc2 = { -1 }; // 编译失败，unsigned char不能够适应负数，对应规则4
unsigned int ui1 = { -1 };  //编译失败，unsigned int不能够适应负数，对应规则4
signed int si1 = { (unsigned int)-1 }; //编译失败，signed int不能够适应-1所对应的
                                   //unsigned int，通常是4294967295，对应规则4
int ii = { 2.0 };  // 编译失败，int不能适应浮点范围，对应规则1
float f1{ x };     // 编译失败，float可能无法适应整数或者互相转换，对应规则3
float f2{ 7 };     // 编译成功，7能够适应float，且float也能转换回整数7，对应规则3
float f3{ cdb };   // 编译成功，99.9能适应float，对应规则2
float f4{ db };    // 编译失败，可能是隐式缩窄转无法表达double，对应规则2
```


## 参考资料

- 《Effective Modern C++》条款七
- 《现代 C++ 语言核心特性解析》 第 9 章