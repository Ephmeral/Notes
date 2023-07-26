# C++ 笔记 | 类型推导
## 函数模版类型推导

对于模版来说，编译的时候会自动推导类型，考虑下面的一个函数模版：

```cpp
template<typename T>
void f(ParamType param);

f(expr);  //从expr中推导T和ParamType
```

ParamType 是在类型 T 的基础上加上 const 或 volatile 限定符（这两简称为 cv 限定符），也可以加上引用修饰；

下面是一个具体的例子：

```cpp
template<typename T>
void f(const T& param);

int x = 0;
f(x); 
```

此时 T 被推导为 int，ParamType 被推导为 const int&；

函数模版类型推导，有以下几种情况：

### 1. ParamType 是一个指针或引用，但不是万能引用

如果 `ParamType` 是一个指针或引用，但不是万能引用，此时：
- 如果 `expr` 类型是引用，会忽略 `expr` 的引用类型；
- 根据 `expr` 的类型和 `ParamType` 来决定 `T` 的类型；

```cpp
// 函数模版
template<typename T>
void f(T& param);

int x = 27;         // x 是 int
const int cx = x;   // cx 是 const int
const int& rx = x;  // rx 是指向作为 const int 的 x 的引用

// 推导结果
f(x);   // T是int，param的类型是int&
f(cx);  // T是const int，param的类型是const int&
f(rx);  // T是const int，param的类型是const int&
```

如果形参类型改为 `const T&`，情况稍有变化：

```cpp
// 函数模版
template <typename T>
void f(const T& param) {}  // param现在是reference-to-const

int x = 27;         // x 是 int
const int cx = x;   // cx 是 const int
const int& rx = x;  // rx 是指向作为 const int 的 x 的引用

// 推导结果
f(x);   // T是int，param的类型是const int&
f(cx);  // T是int，param的类型是const int&
f(rx);  // T是int，param的类型是const int&
```

补充：如何验证类型？

一个是可以通过 IDE 来查看


### 2. ParamType 是一个万能引用

模版使用万能引用也就是 `T&&` 的话，会发生引用折叠：
- 如果 `expr` 是左值，`T` 和 `ParamType` 都会被推导为左值引用；
- 如果 `expr` 是右值，就使用上面一条的推导规则；

```cpp
template <typename T>
void f(T&& param) {}  // param现在是一个通用引用类型

int main() {
	int x = 27;         // x 是 int
	const int cx = x;   // cx 是 const int
	const int& rx = x;  // rx 是指向作为 const int 的 x 的引用

  f(x);   // x是左值，所以T是int&，param类型也是int&
  f(cx);  // cx是左值，所以T是const int&，param类型也是const int&
  f(rx);  // rx是左值，所以T是const int&，param类型也是const int&
  f(27);  // 27是右值，所以T是int，param类型就是int&&
}
```

### 3. ParamType 既不是指针也不是引用

当 `ParamType` 既不是指针也不是引用时，通过传值的方式处理：
- 无论传递什么，param 都会拷贝一份；
- expr 的类型为引用，会忽略这个引用部分；
- 如果是 const 或者 volatile 都会忽略；

```cpp
template <typename T>
void f(T param) {}  // 以传值的方式处理param

int main() {
  int x = 27;          // 如之前一样
  const int cx = x;    // 如之前一样
  const int& rx = cx;  // 如之前一样

  f(x);   // T和param的类型都是int
  f(cx);  // T和param的类型都是int
  f(rx);  // T和param的类型都是int
}
```

上述忽略 const/volatile 是发生在传值给形参过程中的，如果有一个指向 const 对象的 const 指针

```cpp
template<typename T>
void f(T param); //仍然以传值的方式处理param

const char* const ptr = "Fun with pointers";  //ptr是一个常量指针，指向常量对象 

f(ptr); //传递const char * const类型的实参
```

传递的时候会忽略 ptr 本身是 const 属性，所以 `param` 的类型为 `const char*`

### 4. 数组实参

在一些地方，数组会退化了指向它第一个元素的指针，比如函数的形参是数组，此时会退化了指针。如果数组传值给模版参数，它也是会退化为指针：

```cpp
template <typename T>
void f(T param) {}  // 传值形参的模板

const char name[] = "J. P. Briggs";  // name的类型是const char[13]
const char* ptrToName = name;        // 数组退化为指针

f(name);  // name是一个数组，但是T被推导为const char*
```

虽然不能为函数形参声明真正的数组，但是函数形参可以是**指向数组的引用**：

```cpp
template <typename T>
void f(T& param) {}  // 传值形参的模板

const char name[] = "J. P. Briggs";  // name的类型是const char[13]
const char* ptrToName = name;        // 数组退化为指针

f(name);  // T推导为const char[13]
          // ParamType推导为const char(&)[13]
```

可以创建函数模版来导出数组的大小：

```cpp
#include <iostream>
#include <array>
using namespace std;

// 在编译期间返回一个数组大小的常量值（//数组形参没有名字，
// 因为我们只关心数组的大小）
template <typename T, std::size_t N>
constexpr std::size_t arraySize(T (&)[N]) noexcept {
  return N;
}

int main() {
  int keyVals[] = {1, 3, 7, 9, 11, 22, 35};  // keyVals有七个元素

  int mappedVals1[arraySize(keyVals)];              // mappedVals1也有七个
  std::array<int, arraySize(keyVals)> mappedVals2;  // mappedVals2的大小为7
  cout << mappedVals2.size() << endl;
}
```

### 5. 函数实参

传递函数类型的时候，也会退化为函数指针：

```cpp
void someFunc(int, double);  // someFunc是一个函数，类型是void(int, double)

template <typename T>
void f1(T param);  // 传值给f1

template <typename T>
void f2(T& param);  // 传引用给f2

f1(someFunc);  // param被推导为指向函数的指针，类型是void(*)(int, double)
f2(someFunc);  // param被推导为指向函数的引用，类型是void(&)(int, double)
```

### 补充：如何验证参数的类型

前面说的类型推导，如果想在代码中具体查看类型是什么的话，可以利用 C++ 标准库提供的一些函数来判断

首先想到的是 `typeid(T).name()` 来打印类型 T 的名称，但是这里的话因为标准库并未具体规定打印名字是啥，所以不太好观察（下面例子中不再演示）；

另外考虑对应参数类型主要是想判断 T 和 ParamType 的 const 和引用修饰符，标准库提供了 `std::is_const<T>::value` 和 `std::is_reference<T>::value` 类型特征来判断，下面结合例子说明：

来看之前的第一个例子，传递为类型为 T&。这里有个坑点要注意对于 `std::is_reference<const int&>::value` 来说打印的结果是 false，虽然 const int& 是引用类型，这里考虑去除引用类型后来单独判断是否为 const


```cpp
// main.cpp
#include <iostream>
#include <type_traits>
#include <typeinfo>

template <typename T>
void f(T& param) {
  // 使用 decltype 获得param类型，带有引用修饰符
  using ReferenctParamType = decltype(param);
  // 去除引用修饰符，用来单独判断是否为const
  using ConstParamType = typename std::remove_reference<ReferenctParamType>::type;

  std::cout << std::boolalpha;
  std::cout << "T Is const: " << std::is_const<T>::value << std::endl;
  std::cout << "T Is is reference: " << std::is_reference<T>::value
            << std::endl;

  std::cout << "ParamType Is const: " << std::is_const<ConstParamType>::value
            << std::endl;
  std::cout << "ParamType is reference: "
            << std::is_reference<ReferenctParamType>::value << std::endl;
  std::cout << "======================================" << std::endl;
}

int main() {
  int x = 27;         // x 是 int
  const int cx = x;   // cx 是 const int
  const int& rx = x;  // rx 是指向作为 const int 的 x 的引用

  // 推导结果
  f(x);   // T是int，param的类型是int&
  f(cx);  // T是const int，param的类型是const int&
  f(rx);  // T是const int，param的类型是const int&

  return 0;
}
```

输出结果为：

```cpp
$ g++ main.cpp && ./a.out 
T Is const: false
T Is is reference: false
ParamType Is const: false
ParamType is reference: true
======================================
T Is const: true
T Is is reference: false
ParamType Is const: true
ParamType is reference: true
======================================
T Is const: true
T Is is reference: false
ParamType Is const: true
ParamType is reference: true
======================================
```

可以看到上面结果：
- 对于 x 来说，推导的 T 是 int 类型，ParamType 是 int&， 符合预期；
- 对于 cx，推导的 T 为 const int，ParamType 是 const int&，符合预期；
- 对于 rx，推导的 T 为 const int，ParamType 是 const int&，符合预期；

下面再看 ParamType 为 const T& 的结果：

```cpp
// main.cpp
#include <iostream>
#include <type_traits>
#include <typeinfo>

template <typename T>
void f(const T& param) { // 这里改成了 const
  // 使用 decltype 获得param类型，带有引用修饰符
  using ReferenctParamType = decltype(param);
  // 去除引用修饰符，用来单独判断是否为const
  using ConstParamType =
      typename std::remove_reference<ReferenctParamType>::type;

  std::cout << std::boolalpha;
  std::cout << "T Is const: " << std::is_const<T>::value << std::endl;
  std::cout << "T Is is reference: " << std::is_reference<T>::value
            << std::endl;

  std::cout << "ParamType Is const: " << std::is_const<ConstParamType>::value
            << std::endl;
  std::cout << "ParamType is reference: "
            << std::is_reference<ReferenctParamType>::value << std::endl;
  std::cout << "======================================" << std::endl;
}

int main() {
  int x = 27;         // x 是 int
  const int cx = x;   // cx 是 const int
  const int& rx = x;  // rx 是指向作为 const int 的 x 的引用

  // 推导结果
  f(x);   // T是int，param的类型是const int&
  f(cx);  // T是int，param的类型是const int&
  f(rx);  // T是int，param的类型是const int&

  return 0;
}
```

打印结果为：

```cpp
$ g++ main.cpp && ./a.out 
T Is const: false
T Is is reference: false
ParamType Is const: true
ParamType is reference: true
======================================
T Is const: false
T Is is reference: false
ParamType Is const: true
ParamType is reference: true
======================================
T Is const: false
T Is is reference: false
ParamType Is const: true
ParamType is reference: true
======================================
```

也是符合预期的，对于后面的例子可以通过这种方法来进行验证；

## auto

auto 关键字在 C++11 之后重新定义了，用来初始化表达式的时候自动推导变量的类型；理解上面的函数模版类型推导之后，auto 大部分内容是相似的；

对于一个前面说过的函数模版：

```cpp
template<typename T>
void f(ParamType pargm);

f(expr);
```

auto 可以看成是类型 T，最终推导出来的结果和 ParamType 也是相关的，下面是之前的三种情况：

```cpp
#include <iostream>
using namespace std;

int main() {
  int x = 27;
  auto y = x;         // 3.ParamType不是指针也不是引用，y为int类型
  const auto cx = x;  // 3.ParamType不是指针也不是引用，cx为const int类型
  const auto& rx = cx;// 1. ParamType是引用，当不是万能引用，rx为const int&类型

  // 2. ParamType是万能引用
  auto&& uref1 = x;   // x是int左值，所以uref1类型为int&
  auto&& uref2 = cx;  // cx是const int左值，所以uref2类型为const int&
  auto&& uref3 = 27;  // 27是int右值，所以uref3类型为int&&
}
```

对于数组和函数退化成指针也是一样的：

```cpp
#include <iostream>
using namespace std;

int main() {
  const char name[] = "R. N. Briggs";  // name的类型是const char[13]

  auto arr1 = name;   // arr1的类型是const char*
  auto& arr2 = name;  // arr2的类型是const char (&)[13]

  void someFunc(int, double);  // someFunc是一个函数，类型为void(int, double)

  auto func1 = someFunc;   // func1的类型是void (*)(int, double)
  auto& func2 = someFunc;  // func2的类型是void (&)(int, double)
}
```

这样查看这些变量的具体类型，可以利用 IDE 根据来进行查看，VSCode 上面是可以直接看见类型的（安装 C/C++ 那个插件应该就可以了）。

### 统一初始化

C++11 之后用大括号来表示统一初始化，对于声明一个已知类型的变量来说，不会发生奇怪的事情，但是使用 auto 类型推导的时候，推导的结果可能不是那么的直观，如下：

```cpp
#include <iostream>
using namespace std;

int main() {
  // 下面4个值都为27
  int x1 = 27;
  int x2(27);
  int x3 = {27};
  int x4{27};

  auto x5 = 27;    // int, 27
  auto x6(27);     // int, 27
  auto x7 = {27};  // std::initializer_list<int>，只有一个元素，值为27
  auto x8{27};     // int, 27
}
```

对于 x7 来说推导的结果是 `std::initializer_list<int>`，而不是 int 类型；

auto 和统一初始化列表组合的推导规则如下：
- （1）直接使用列表初始化，列表中必须为单元素，否则无法编译，`auto` 类型被推导为单元素的类型。
- （2）用等号加列表初始化，列表中可以包含单个或者多个元素，`auto` 类型被推导为 `std::initializer_list<T>`，其中 `T` 是元素类型。请注意，在列表中包含多个元素的时候，元素的类型必须相同，否则编译器会报错。

```cpp
auto x1 = {1, 2};    // x1类型为 std::initializer_list<int>
auto x2 = {1, 2.0};  // 编译失败，花括号中元素类型不同
auto x3{1, 2};       // 编译失败，不是单个元素
auto x4 = {3};       // x4类型为std::initializer_list<int>
auto x5{3};          // x5类型为int
```

### 其他一些规则

1. 当 `auto` 关键字声明多个变量的时候，编译器会从左往右推导，以最左边的表达式判断 auto 的类型，如果推导的类型不匹配，会报错：

```cpp
// mulit_variant.cpp
#include <iostream>
using namespace std;

int main() {
  int n = 5;
  auto *p = &n, q = n;  // p为int*，q为int

  auto *pp = &n, qq = 10.0;  // 类型不匹配
}
```

报错结果：

```shell
$ g++ mulit_variant.cpp 
mulit_variant.cpp: In function ‘int main()’:
mulit_variant.cpp:8:3: error: inconsistent deduction for ‘auto’: ‘int’ and then ‘double’
   auto *pp = &n, qq = 10.0;  // 类型不匹配
   ^~~~
```

2．当使用条件表达式初始化 `auto` 声明的变量时，编译器总是使用表达能力更强的类型：

```cpp
auto i = true ? 5 : 8.0;    // i的数据类型为double
```

在上面的代码中，虽然能够确定表达式返回的是 `int` 类型，但是 i 的类型依旧会被推导为表达能力更强的类型 `double`。

3．`auto` 无法声明非静态成员变量：

```cpp
struct sometype {
    auto i = 5;    // 错误，无法编译通过
};
```

在 C++11 中静态成员变量是可以用 `auto` 声明并且初始化的，不过前提是 `auto` 必须使用 `const` 限定符：

```cpp
struct sometype {
    static const auto i = 5;
};
```

在 C++17 标准中，对于静态成员变量，`auto` 可以在没有 `const` 的情况下使用，例如：

```cpp
struct sometype {
    static inline auto i = 5;    // C++17
};
```

### 返回值类型推导

C++14 允许 auto 作为函数返回值，会自动推导出相应的类型：

```cpp
auto sum(int x, int y) {
	return x + y;
}
```

多个返回值的时候，返回类型需要统一，否则会失败：

```cpp
// 类型推导失败，有冲突
auto add(long x, long y) {
  if (x < 0) {
    return 0;  // 返回类型为int，改成0L即可
  }
  return x + y;  // 返回类型为long
}
```

返回值不能推导统一初始化列表：

```cpp
auto createInitList() { 
    return {1, 2, 3};  //错误！不能推导{ 1, 2, 3 }的类型
}
```

### 作为 Lambda 函数形参

C++14 允许 auto 作为 Lambda 表达式的形参：

```cpp
// lambda.cpp
#include <iostream>
using namespace std;

int main() {
    auto sum = [](auto x, auto y) {
        return x + y;
    };

    auto z = sum(10, 20.5); // x推导为int，y推导为double，返回值推导为double
    std::cout << "z = " << z << std::endl; // z = 30.5
}
```

注意：auto 作为 Lambda 表达式也不能推导统一初始化列表：

```cpp
std::vector<int> v;
auto resetV = [&v](const auto& newValue) { v = newValue; };  // C++14
resetV({1, 2, 3});  // 错误！不能推导{ 1, 2, 3 }的类型
```

## decltype

decltype 用来获取一个的类型而不实际执行该表达式。它通常在模板编程和泛型编程中用于声明变量的类型，下面是些示例：

```cpp
// example.cpp
#include <iostream>
using namespace std;

struct Point {
  int x, y;
};

bool f(const Point& p) {
  if (p.x >= 0 && p.y >= 0) {
    return true;
  }
  return false;
}

int main() {
  const int i = 0;
  decltype(i) j = 0;  // j类型为const int

  decltype(f) fn;  // fn类型为bool(const Point&);

  Point p1(10, 20);
  decltype(p1) p2;  // p2类型为Point

  if (fn(p1)) {
    //...
  }

  decltype(fn(p1)) res;  // res类型为bool
}
```

### 尾置返回类型

decltype 的一个用途是用在尾置返回类型语法中：

```cpp
#include <iostream>
using namespace std;

template <class T1, class T2>
auto sum(T1 t1, T2 t2) -> decltype(t1 + t2) {
  return t1 + t2;
}

int main() {
    auto x = sum(4, 2.0); // 返回值推导为double
}
```

上面说到了 C++14 允许 auto 自动推导返回值的类型，所以这里看来 decltype 并没有什么必要。看下面这个例子，假设通过一个模版函数得到容器中某个元素，然后想修改这个元素，如果是用 auto 自动推导返回值类型的话：

```cpp
template<typename Container, typename Index>
auto getItem(Container &c, Index id) 
{
    return c[id];
}

int main() {
	std::vector<int> nums{1, 2, 3, 4, 5};

    getItem(nums, 0) = 10; // 编译错误，返回值不是引用类型
    std::cout << nums[0] << std::endl;
}
```

如果用 decltype 就可以完成上述的工作：

```cpp
template<typename Container, typename Index>
auto getItem(Container &c, Index id) -> decltype(c[id]) 
{
    return c[id];
}

int main() {
    auto x = sum(4, 2.0); // 返回值推导为double

    std::vector<int> nums{1, 2, 3, 4, 5};

    getItem(nums, 0) = 10;
    std::cout << nums[0] << std::endl;
}
```

这是因为 auto 自动类型推导的时候会忽略引用类型，而 decltype 会保留返回的引用类型；

### 推导规则

decltype 推导规则如下：

1. 如果 `e` 是一个未加括号的标识符表达式（结构化绑定除外）或者未加括号的类成员访问，则 `decltype(e)` 推断出的类型是 `e` 的类型 `T`。如果并不存在这样的类型，或者 `e` 是一组重载函数，则无法进行推导；
2. 如果 `e` 是一个函数调用或者仿函数调用，那么 `decltype(e)` 推断出的类型是其返回值的类型；
3. 如果 `e` 是一个类型为 `T` 的左值，则 `decltype(e)` 是 `T&`；
4. 如果 `e` 是一个类型为 `T` 的将亡值，则 `decltype(e)` 是 `T&&`；
5. 如果 `e` 是一个类型为 `T` 的纯右值 ，则 `decltype(e)` 是 `T`；

```cpp
struct A {
  double x;
};
const A* a;

decltype(a->x) y;        // y 的类型是 double（其声明类型），规则1
decltype((a->x)) z = y;  // z 的类型是 const double&（左值表达式），规则4

int i = 0;
decltype(i) j;    // j的类型是int，规则1
decltype((i)) k;  // k的类型是int&，规则3

const int&& foo();
decltype(foo()) fn;  // fn类型是 cont int&&，规则2
```

下面是更加复杂的一些例子：

```cpp
int i;
int* j;
int n[10];
// i = 0，返回的是i，是左值，规则3，decltype(i=0)推导类型为int&
decltype(i = 0);

// 补充，逗号表达式，先计算左边的，再计算右边的，最后返回值是右边的
// 0, i表达式返回的是i，是左值，规则3，decltype(0,i)推导类型为int&
decltype(0, i);
// i, 0表达式返回的是0，是纯右值，规则5，decltype(i,0)推导类型为int
decltype(i, 0);

// n[5]是左值，规则3，decltype(n[5])推导类型为int&
decltype(n[5]);

// *j是个左值（可以赋值），规则3，decltype(*j)推导类型为int&
decltype(*j);

// static_cast<int&&>(i)转换后是一个将亡值，decltype(static_cast<int&&>(i))推导类型为int&&
decltype(static_cast<int&&>(i));

// i++，返回的是原来的i，之后i再加1，返回的是纯右值，规则5，decltype(i++)推导类型为int
decltype(i++);

// ++i，先执行i加1，然后再返回i，返回的是左值，规则3，decltype(++i)推导类型为int&
decltype(++i);

// "hello world"是常量数组的左值，规则3，decltype("hello world")推导类型为
// const char(&)[12]
decltype("hello world");
```

通常情况下，`decltype(e)` 所推导的类型会同步 `e` 的 `cv` 限定符，特殊情况就是规则 1，当 e 是未加括号的成员变量的时候，会忽略父对象的 cv 限定符，给 e 加上括号后结果会不一样：

```cpp
struct A {
    double x;
};
const A* a = new A();
decltype(a->x);    // decltype(a->x)推导类型为double, const属性被忽略
decltype((a->x));  // decltype((a->x))推导类型为const double&
```

### decltype(auto)

C++14 允许使用 `decltype(auto)`，它告诉编译器用 decltype 的推导规则来推导 auto，`decltype(auto)` 必须单独声明，不能和引用、指针、cv 限定符一起使用，下面是个例子：

```cpp
int i;
int&& f();
auto x1a = i;                 // x1a推导类型为int
decltype(auto) x1d = i;       // x1d推导类型为int

auto x2a = (i);               // x2a推导类型为int
decltype(auto) x2d = (i);     // x2d推导类型为int&

auto x3a = f();               // x3a推导类型为int
decltype(auto) x3d = f();     // x3d推导类型为int&&

auto x4a = {1, 2};            // x4a推导类型为std::initializer_list<int>
decltype(auto) x4d = {1, 2};  // 编译失败, {1, 2}不是表达式

auto* x5a = &i;               // x5a推导类型为int*
decltype(auto)* x5d = &i;     // 编译失败，decltype(auto)必须单独声明
```

如果函数返回值使用 decltype(auto) 的话，下面两种情况会有所区别：

```cpp
decltype(auto) f1() {
	int x = 0;
	// ...
	return x; // 返回int
}

decltype(auto) f2() {
	int x = 0;
	// ...
	return (x); // 返回int&，但是这样引用了一个局部变量，会产生未定义行为
}
```

## 参考资料
- 《Effective Modern C++》第 1-2 章
- 《现代 C++ 语言核心特性解析》 第 3-4 章