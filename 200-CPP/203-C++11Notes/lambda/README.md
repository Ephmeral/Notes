## Lambda 概念

Lambda 表达式它本质上是一个表达式（废话），它的形式如下：

```cpp
[捕获列表](参数列表) 限定修饰符 异常修饰符 -> 返回类型 {函数体}
```

Lambda 表达式的作用是产生一个闭包（enclosure），所谓闭包即 Lambda 表达式创建的运行时对象，闭包会根据 Lambda 表达式捕获模式，持有被捕获数据的副本或者引用。

Lambda 表达式会生成一个匿名类（也称之为闭包类），每个编译器都会为 Lambda 表达式生成唯一的闭包类，**Lambda 当中的语句会成为闭包类的成员函数中的可执行指令**。

下面举个例子：

```cpp
#include <iostream>
using namespace std;

int main() {
    int x = 10;
    int y = 20;
    // Lambda表达式，按值捕获x
    auto c = [x](int y) {
        return x * y >= 100;
    };

    cout << "sizeof(c) = " << sizeof(c) << endl;
    cout << "c = " << c(y) << endl;
    return 0;
}
```

将上面的代码放到 [C++ Insights (cppinsights.io)](https://cppinsights.io/) 网站可以看到生成了下面的一个闭包类：

```cpp
class __lambda_8_14
{
public: 
inline /*constexpr */ bool operator()(int y) const
{
  return (x * y) >= 100;
}

private: 
int x;

public:
__lambda_8_14(int & _x)
: x{_x}
{}

};
```

按值捕获的 x 作为成员变量，同时 Lambda 表达式当中的语句放在了 `operator()` 操作符重载函数当中了。

## 捕获列表
### 作用域

捕获列表中的变量有两个作用域：
- Lambda 表达式定义的函数作用域，这是为了捕获变量，在这个作用域之外的变量无法捕获；
- Lambda 表达式函数体的作用域，也就是 Lambda 表达式当中使用变量的作用域。

C++ 标准规定捕获的变量必须是一个自动存储类型（即非静态的局部变量），全局变量和局部静态变量是无法捕获的，但是可以直接使用。例如：

```cpp
int x = 0;

int main() {
    int y = 0;
    static int z = 0;
    auto foo = [x, y, z] {}; // 这里编译出错
    // 直接使用即可x和y即可
	auto foo1 = [y] { return x + y + z; }; 
}
```

### 捕获值和捕获引用

捕获列表的捕获方式分为**捕获值和捕获引用**，捕获值的写法是在 `[]` 当中直接写入变量名，有多个变量用逗号隔开，如果是捕获引用在变量名前面加上 `&` 即可，例如：

```cpp
int main() {
    int x = 5, y = 8;
    auto foo = [x, y] { return x * y; };
    auto foo2 = [&x, &y] { return x * y; };
}
```

捕获值和捕获引用一些区别：
- 1）**如果对捕获后的变量进行赋值，捕获值会编译失败**，因为 Lambda 表达式捕获的变量默认是常量，或者说 **Lambda 是一个常量函数**（类似于常量成员函数），无法改变捕获变量的值；捕获引用的方式是可以编译成功的；
	- 如果想对捕获值方式进行变量修改，可以在 Lambda 表达式加上 mutable 修饰符，取消 Lambda 常量成员函数的形式。
- 2）捕获值的时候，修改的是捕获变量的副本，修改 Lambda 表达式内部的变量不会影响外面的变量，但是对于捕获引用来说，内部进行修改，外部也会改变。
- 3）捕获值的变量在 Lambda 表达式定义的时候已经固定下来了，无论函数在 Lambda 表达式定义后如何修改外部变量的值，Lambda 表达式捕获的值都不会变化。

### 特殊捕获方法

Lambda 表达式除了捕获变量外，还有 3 种特殊的捕获方法：

1. `[this]` —— 捕获 this 指针，捕获 this 指针可以让我们使用 this 类型的成员变量和函数。
2. `[=]` —— 捕获 lambda 表达式定义作用域的全部变量的值，包括 this。
3. `[&]` —— 捕获 lambda 表达式定义作用域的全部变量的引用，包括 this。

捕获 this 指针的作用是因为在类成员函数当中定义 Lambda 表达式的话，是无法直接使用类的成员变量的或者调用类的成员函数的，使用捕获 this 指针之后就可以使用了，例如：

```cpp
#include <iostream>

class A {
public:
  void print() { std::cout << "class A" << std::endl; }

  void test() {
    auto foo = [this] {
      print();
      x = 5;
    };
    foo();
  }

private:
  int x;
};

int main() {
  A a;
  a.test();
}
```

### 广义捕获

广义捕获是 C++14 提出来的，它是两种捕获方式，第一种称为简单捕获，这种捕获就是我们在前文中提到的捕获方法，即 `[identifier]`、`[&identifier]` 以及 `[this]` 等。第二种叫作初始化捕获，它解决了简单捕获的一个重要问题，即只能捕获 lambda 表达式定义上下文的变量，而无法捕获表达式结果以及自定义捕获变量名。

```cpp
int main() {
    int x = 5;
    auto foo = [x = x + 1]{ return x; };
}
```

初始化捕获的两个应用场景：
- 1）使用移动操作减少代码运行的开销；
- 2）异步调用时复制 this 对象，防止 lambda 表达式被调用时因原始 this 对象被析构造成未定义的行为；

## Lambda 表达式实现原理

Lambda 表达式和函数对象（仿函数）很相似，**Lambda 表达式在编译期会由编译器自动生成一个闭包类，在运行时由这个闭包类产生一个对象，我们称它为闭包**。Lambda 表达式捕获的变量会作为闭包类的成员变量。

最开始的例子当中可以看到 Lambda 表达式，编译器会自动为它生成一个拥有 `operator()` 重载操作符的匿名类，而它就是函数对象的特征，所以在某种程度上来说，Lambda 表达式是 C++11 给我们提供的一块语法糖而已，只不过使用 Lambda 表达式让代码编写更加简洁。

## 无状态 Lambda 表达式

所谓无状态 Lambda 表达式指的是 Lambda 表达式当中没有捕获任何的变量，C++ 当中对于无状态的 Lambda 表达式是可以隐式转换为函数指针的，某些场景下有用。

```cpp
void f(void (*)()) {}
void g() {
  f([] {});// 编译成功
} 
```

## 参考资料

[C++11 lambda表达式使用及解析 - 知乎 (zhihu.com)](https://zhuanlan.zhihu.com/p/468616999)
- 《Effective Modern C++》第 6 章
- 《现代 C++ 语言核心特性解析》 第 7 章
