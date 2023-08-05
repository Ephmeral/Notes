# override 关键字
## 重载、重写和隐藏

1. 重载：同一个类中有 2 个或以上的函数，它们函数名相同，但是形参列表不一样；
2. 重写：又称覆盖，是指派生类覆盖了基类的虚函数，覆盖的函数必须相同的函数名、形参列表以及返回类型；
3. 隐藏：派生类的成员函数和基类的某个函数名一样（可以是虚函数也可以不是），但是形参列表不一样，此时基类函数会被隐藏。如果想使用基类函数可以用 using 关键字导入；

这里形参列表不同，指的是形参的类型或者数量有区别。另外注意函数重载不允许通过返回类型来区分，即如果只有返回类型不一样，其他的都一样，不是函数重载，此时会发生编译错误（重复定义）；

## override

先补充一下完整的重写要求：
- 基类函数必须是 `virtual`；
- 基类和派生类函数名必须完全一样（除非是析构函数)；
- 基类和派生类函数形参类型必须完全一样；
- 基类和派生类函数常量性 `const`ness 必须完全一样；
- 基类和派生类函数的返回值和异常说明（_exception specifications_）必须兼容；

重写（覆盖）基类的虚函数，很有可能出现问题，比如由于粗心导致派生类的函数名写错了一个字母，编译器是无法检测到这个问题的：

```cpp
class Base {
 public:
  virtual void some_func() {}
  virtual void foo(int x) {}
  virtual void bar() const {}
  void baz() {}
};

class Derived : public Base {
 public:
  virtual void sone_func() {}  // 字母写错了，some_func -> sone_func
  virtual void foo(int &x) {}  // 形参列表不同
  virtual void bar() {}        // 常量性不同
  virtual void baz() {}        // 基类不是虚函数，不是重写
};
```

C++11 引入 *override* 说明符，来告诉编译器这个虚函数需要重写基类的虚函数，一旦编译器发现不符合重写规则，就会给出错误提示：

```cpp
// vfunc2.cpp
#include <iostream>
using namespace std;

class Base {
 public:
  virtual void some_func() {}
  virtual void foo(int x) {}
  virtual void bar() const {}
  void baz() {}
};

class Derived : public Base {
 public:
  virtual void sone_func() override {}  // 字母写错了，some_func -> sone_func
  virtual void foo(int &x) override {}  // 形参列表不同
  virtual void bar() override {}        // 常量性不同
  virtual void baz() override {}        // 基类不是虚函数，不是重写
};
```

编译错误如下：

```shell
$ g++ vfunc2.cpp 
vfunc2.cpp:14:16: error: ‘virtual void Derived::sone_func()’ marked ‘override’, but does not override
   virtual void sone_func() override {}  // 字母写错了，some_func -> sone_func
                ^~~~~~~~~
vfunc2.cpp:15:16: error: ‘virtual void Derived::foo(int&)’ marked ‘override’, but does not override
   virtual void foo(int &x) override {}  // 形参列表不同
                ^~~
vfunc2.cpp:16:16: error: ‘virtual void Derived::bar()’ marked ‘override’, but does not override
   virtual void bar() override {}        // 常量性不同
                ^~~
vfunc2.cpp:17:16: error: ‘virtual void Derived::baz()’ marked ‘override’, but does not override
   virtual void baz() override {}        // 基类不是虚函数，不是重写
                ^~~
```

## final

C++ 中可以让纯虚函数使得派生类必须重写这个纯虚函数，但是没有关键字可以阻止派生类去继承基类的虚函数。

C++11 引入 final 说明符解决了这个问题：

```cpp
class Base {
public:
  virtual void foo(int x) {}
};

class Derived : public Base {
public:
  void foo(int x) final {};
};

class Derived2 : public Derived {
public:
  void foo(int x) {}; // 这里编译错误
};
```

上面代码中派生类 Derived 可以重写基类的虚函数，但是 Derived 添加了 final 说明符，所以派生类的派生类 Derived2 无法在重写父类的虚函数；

## 上下文关键字

前面说到 override 和 final 的时候，这里用的是说明符，而不是关键字。因为这两个是 C++ 引入的上下文关键字（_contextual keywords_），特点是只有当在特定的上下文的时候，才会被视为关键字。override 和 final 只在成员函数的结尾才会被视为关键字，如果 C++98 的老代码中用到了这两个名字，C++11 中也仍然可以正常运行：

```cpp
class Warning {           // C++98潜在的传统类代码
public:
    void override() {}    // C++98和C++11都合法（且含义相同）
    void final() {}       // C++98和C++11都合法（且含义相同）
    // ...
};
```

## 成员函数引用限定

C++11 添加了一个新的限定符，成员函数的引用限定，它可以限定成员函数只能用于左值或右值，下面是个简单的例子：

```cpp
// reference.cpp
#include <iostream>
using namespace std;

class Widget {
 public:
  // //只有*this为左值的时候才能被调用
  void doWork() & { std::cout << "this为左值，Widget::doWork &" << std::endl; }
  // 只有*this为右值的时候才能被调用
  void doWork() && { std::cout << "this为右值，Widget::doWork &&" << std::endl; }
};

// 工厂函数（返回右值）
Widget&& makeWidget() {
  Widget w;
  return std::move(w);
}

int main() {
  Widget w;  // 普通对象（左值）

  w.doWork();  // 调用被左值引用限定修饰的版本（即Widget::doWork &）
  makeWidget().doWork();  // 调用被右值引用限定修饰的版本（即Widget::doWork &&）
}
/*
$ g++ reference.cpp && ./a.out 
this为左值，Widget::doWork &
this为右值，Widget::doWork &&
*/
```

成员引用限定符的用处，现在有个 `Widget` 类有一个 `std::vector` 数据成员，有一个 `data` 成员函数返回这个成员变量：

```cpp
// reference_qualifier2.cpp 
#include <iostream>
#include <vector>
using namespace std;

using DataType = std::vector<int>;

class Widget {
public:
  DataType& data() { 
    std::cout << "拷贝data" << std::endl;
    return values; 
  }

private:
  DataType values;
};

// 工厂函数（返回右值）
Widget&& makeWidget() {
  Widget w;
  return std::move(w);
}

int main() {
  Widget w;  // 普通对象（左值）
  auto v1 = w.data(); // 拷贝 w.values到 v1
  auto v2 = makeWidget().data(); // 拷贝 Widget 里面的值到 v2
}
```

上面代码中，v1 是左值引用，v2 是右值引用，v1 和 v2 本身都是左值，所以对于成员变量 values 会依次拷贝到 v1 和 v2 中，但是对于 v2 来说这里应该使用移动而不是拷贝，使用成员函数引用限定符可以解决这个问题：

```cpp
// reference_qualifier3.cpp 
#include <iostream>
#include <vector>
using namespace std;

using DataType = std::vector<int>;

class Widget {
public:
  DataType& data() &{ 
    std::cout << "拷贝data" << std::endl;
    return values; 
  }
  DataType data() && {
    std::cout << "移动data" << std::endl;
    return std::move(values);
  }

private:
  DataType values;
};

// 工厂函数（返回右值）
Widget&& makeWidget() {
  Widget w;
  return std::move(w);
}

int main() {
  Widget w;  // 普通对象（左值）
  auto v1 = w.data(); // 拷贝 w.values到 v1
  auto v2 = makeWidget().data(); // 移动 Widget 里面的值到 v2
}
/*
$ g++ reference_qualifier3.cpp && ./a.out 
拷贝data
移动data
*/
```

成员函数引用和 const 修饰类似，都是修饰 \*this 对象的性质，如果 const 和成员函数引用限定同时存在的话，C++ 标准规定 const 写在前面：

```cpp
// const_ref_qualifier.cpp
#include <iostream>
using namespace std;

class Widget {
public:
    void func1() const {
    }   
    void func2() const & {
    } 
    void func3() & const { // 这里编译错误
    }
};

int main() {
    Widget w;
}
```

编译错误：

```shell
$ g++ const_ref_qualifier.cpp 
const_ref_qualifier.cpp:11:18: error: expected ‘;’ at end of member declaration
     void func3() & const { // 这里编译错误
                  ^
                   ;
const_ref_qualifier.cpp:11:26: error: expected unqualified-id before ‘{’ token
     void func3() & const { // 这里编译错误
```

## 参考资料

- 《Effective Modern C++》条款十二
- 《现代 C++ 语言核心特性解析》 第 16 章