# 《Effective C++》笔记
# 1 让自己习惯 C++

## 条款 01：视 C++ 为一个语言联邦

将 C++ 视为一个由相关语言组成的联邦，而非单一语言，主要为以下四种：

- C 语言：包括区块、语句、预处理器、内置类型、数组、指针等。
- 面向对象：即 C with classes 包括类（构造、析构函数）、封装、继承、多态、虚函数（动态绑定）等。
- 泛型编程：即模版，以及更加高级的 template metaprogramming（TMP，模版元编程）。
- STL：C++ 标准库，主要包含：容器、迭代器、算法、分配器、适配器和仿函数。

## 条款 02：尽量以 const, enum, inline 替换 \#define

`#define` 是在预处理阶段进行简单的文本替换，它不会涉及类型检查等，在编译期间也无法察觉到宏定义的常量，推荐使用 const 常量：

```cpp
const double Aspect_ratio = 1.653;

// 替代下面的宏定义
#define ASPECT_RATIO 1.653
```

可以将 const 常量定义为类的成员变量，同时保证这个常量最多只有一份，所以可以设置为 static 的：

```cpp
class GamePlayer {
public:
    static const int NumTurns = 5; // 常量声明式
    int scores[NumTurns];
};
```

上面的 NumTurns 只是声明，而不是定义，如果不取它的地址，就不用进行定义，但是如果需要取它的地址，必须提供一个额外的定义：

```cpp
// 这个式子需要放在源文件，而不是头文件
const int GamePlayer::NumTurns; // NumTruns的定义
```

可以用枚举类型来代替常量类型，枚举类型还是模版元编程的基础技术（见条款 48）：

```cpp
class GamePlayer {
public:
    enum { NumTurns = 5 }; // 枚举类型
    int scores[NumTurns];    
};
```

使用宏来当作函数使用，看似不会因为系统调用带来开销，但是实际上可能会有隐患，下面是因为宏替换的时候，++a 一起替换到表达式当中了，当 ++a 的结果比 b 大的时候，会再调用一次 ++a：

```cpp
#define CALL_WITH_MAX(a, b) f((a) > (b) ? (a) : (b))

int a = 5, b = 0;
int a = 5, b = 0;
CALL_WITH_MAX(++a, b); // a被累加两次
CALL_WITH_MAX(++a, b + 10); // a被累加一次
```

更好的方法是采用内联模版函数：

```cpp
template<typename T>
inline void CallWithMax(const T& a, const T& b) {
    f(a > b ? a : b);
}
```

## 条款 03：尽可能使用 const

const 允许指定一个变量是不可修改的，对于指针，可以让指针自身、指针指向的对象，或者两者都是（不是）const 的，**关键字 const 出现在星号左边，指针指向的对象时常量；出现在星号右边，则指针本身是常量**。

```cpp
char greeting[] = "Hello";
char* p = greeting;                // 指针可修改，数据可修改
const char* p = greeting;          // 指针可修改，数据不可修改
char const* p = greeting;          // 指针可修改，数据不可修改
char* const p = greeting;          // 指针不可修改，数据可修改
const char* const p = greeting;    // 指针不可修改，数据不可修改
```

STL 迭代器作用类似于 T* 指针，也分为迭代器本身是 const 还是迭代器指向的对象时 const 的：

```cpp
const std::vector<int>::iterator iter = vec.begin();    // 迭代器不可修改，数据可修改
std::vector<int>::const_iterator iter = vec.begin();    // 迭代器可修改，数据不可修改
```

函数返回一个常量，可以避免用户使用错误导致意外发生：

```cpp
const Rational operator*(const Rational& lhs, const Rational& rhs);

// 下面是用户可能的错误写法
Rational a, b, c;
...
if (a * b = c) ... // 用户错误的将比较写成了赋值，返回const可以检查出来
```

成员函数的常量性不同，也可以被重载：

```cpp
class TextBlock {
public:
    const char& operator[](std::size_t position) const {    // const对象使用的重载
        return text[position];
    }

    char& operator[](std::size_t position) {                // non-const对象使用的重载
        return text[position];
    }

private:
    std::string text;
};
```

在使用 const 对象的时候，会调用 const 版本的重载函数：

```cpp
void print(const Textblock& ctb) {
    std::cout << ctb[0];            // 调用 const TextBlock::operator[]
}
```

另外注意重载的 `operator[]` 返回的是 char&，而不是 char，如果是 char 类型则返回的是副本，而不是原来的 text[0]。

编译器处理 const 成员函数的时候，会认为这个成员函数不会改变对象当中任何变量，也就是所谓的 bitwise const，但是实际上我们编写的代码是 logical const，即一个 const 成员函数可以修改它处理对象内部的某些成员，但是只在客户端感受不到的情况下才如此。

比如我们想用一个 const 成员函数返回长度，但是这个长度需要在第一次调用的时候进行初始化，这个时候需要用 mutable 来修饰成员变量，从而来释放掉编译器的 bitwise const 语义：

```cpp
class CTextBlock {
public:
    std::size_t Length() const;

private:
    char* pText;
    mutable std::size_t textLength;
    mutable bool lengthIsValid;
};

std::size_t CTextBlock::Length() const {
    if (!lengthIsValid) {
        textLength = std::strlen(pText);    // 可以修改mutable成员变量
        lengthIsValid = true;               // 可以修改mutable成员变量
    }
    return textLength;
}
```

const 成员函数不允许调用非 const 成员函数，因为后者可能会修改类中的成员变量，但是非 const 成员函数时可以调用 const 成员函数，当有两个重载的成员函数时，可以在非 const 成员函数中使用转型，从而调用 const 成员函数的代码，来达到简化代码的目的：

```cpp
class TextBlock {
public:
    const char& operator[](std::size_t position) const {

        // 假设这里有非常多的代码

        return text[position];
    }

    char& operator[](std::size_t position) {
        return const_cast<char&>(static_cast<const TextBlock&>(*this)[position]);
    }

private:
    std::string text;
};
```

这里简单解释一下转型的代码 `const_cast<char&>(static_cast<const TextBlock&>(*this)[position])`：
- 首先 `static_cast` 将 `*this` 加上了 const 属性，这样才能调用对应 const 版本的重载函数；
- 调用 const 版本的函数返回值为 `const char&` 类型的，通过 `const_cast` 会去掉常量性，最后返回 `char &` 类型。