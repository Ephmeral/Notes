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

## 条款 04：确定对象在使用前已被初始化

对于内置类型，在使用之前最好手动进行初始化，避免使用未初始化的变量导致未定义的行为。

```cpp
int x = 0;
const char* text = "A C-style string";

double d;
std::cin >> d;
```

对于内置类型之外的类型，初始化的工作由构造函数来完成，构造函数需要确保每个成员变量都进行初始化，注意区分赋值和初始化两者的区别，构造函数当中可以采用成员初始化列表：

```cpp
ABEntry::ABEntry(const std::string& name, const std::string& address,
                 const std::list<PhoneNumber>& phones)
    : theName(name),
      theAddress(address),
      thePhones(phones),
      numTimesConsulted(0) {
/* 以下的做法为赋值，而非初始化，一般不建议构造函数中采用赋值
	theName = name;
	theAddress = address;
	thePhones = phones;
	numTimesConsulted = 0;
*/
}
```

为什么更加推荐成员初始化列表？
- 如果是赋值的话，对于类成员对象来说，会先自动调用默认的构造函数，然后再调用拷贝赋值运算符。
- 如果是采用成员初始化列表的话，会直接调用拷贝构造函数，一般来说直接调用拷贝构造函数的效率会更高。
- 某些情况下必须使用初始化、而不能是赋值：比如 const 或引用类型。

C++ 成员初始化的顺序是固定的：
- 基类的对象总是优先于派生类对象的初始化；
- 类成员变量总是按照声明的次序被初始化。（注：：**成员初始化列表当中最好按照声明的次序书写**）

**静态对象的初始化**：
- 静态对象（即 static），它生命周期会从构造出来一直到程序结束才终止。在函数内的 static 对象称之为局部静态对象，其他的（全局的、namespace 作用域内、类中、文件作用域）都统称为非局部静态对象。
- **编译单元**是指产出单一目标文件的哪些源码，基本上它是单一源文件加上包含的头文件（#include）。

**不同编译单元的非局部静态对象的初始化顺序是未定义的。** 如果一个编译单元的初始化依赖于另外一个编译单元的初始化，可能会出现未初始化静态对象的情况，例如下面无法保证 tempDir 初始化之前一定先初始化了 tfs：

```cpp
// 下面这种做法
// File 1
extern FileSystem tfs;

// File 2
class Directory {
public:
    Directory() {
        FileSystem disk = tfs;
    }
};

Directory tempDir;
```

考虑采用 Singleton 模式，将每个函数的非局部静态变量放到自己的专属函数当中，然后返回一个指向该对象的引用，**C++ 会保证局部静态变量在函数调用期间、首次语句该对象的定义的时候就初始化**，如果从来没调用过这个函数，也不会有构造和析构的成本：

```cpp
FileSystem& tfs() {
    static FileSystem fs;
    return fs;
}

Directory& tempDir() {
    static Directory td;
    return td;
}
```

---
# 2 构造/析构/赋值运算

## 条款 05：了解 C++ 默默编写并调用哪些函数

对于空类来说，如果没有声明任何的函数，编译器会自动声明一个拷贝构造函数、拷贝赋值运算符以及一个析构函数。如果没有声明任何的构造函数，也会生成一个默认的构造函数，这些函数都是 public 且 inline 的，而且**仅当使用到这些函数的时候，编译器才会生成**。（注：C++11 之后还会生成移动构造函数和移动赋值运算符）

```cpp
class Empty {};

Empty e1; // 默认构造函数
Empty e2(e1); // 拷贝构造函数
e2 = e1; // 拷贝赋值运算符
Empty e3 = std::move(e1); // 移动构造函数 C++11
e3 = std::move(e2); // 移动赋值运算符 C++11
```

编译器生成的拷贝构造函数、拷贝赋值运算符会单纯的将非静态成员变量拷贝到目标对象当中（是浅拷贝），如果成员变量定义了拷贝构造函数也会调用成员变量的拷贝构造函数。

在一些情况下，拷贝赋值运算符是不允许存在的，例如：包含引用的成员、内含 const 成员、以及基类的拷贝赋值运算定义为 private 的，编译器会拒绝为派生类生成拷贝赋值运算符。

```cpp
class NamedObject {
private:
    std::string& nameValue;
};
```

## 条款 06：若不想使用编译器自动生成的函数，就该明确拒绝

如果想让某个类对象不允许进行拷贝，条款 05 提到编译器会自动合成相应的拷贝构造和拷贝赋值运算符，可以将类的拷贝构造和拷贝赋值运算符定义为 private 来达到这一目的：

```cpp
class HomeForSale {
public:
    ...
private:
    ...
    HomeForSale(const HomeForSale&);		//只有声明，没有定义
    HomeForSale& operator=(const HomeForSale&);
}
```

上面只声明了拷贝构造函数和拷贝赋值运算符，但是在类当中还是有可能会使用拷贝操作，这个时候链接器会报错，如果想在编译器编译的时候就报错的话，考虑为这个类定义一个 uncopyable 的父类：

```cpp
class Uncopyable {
public:
	Uncopyable() {} // 允许派生类对象构造和析构
	~Uncopyable() {}
private:
    Uncopyable(const Uncopyable&); // 但是禁止拷贝
    Uncopyable& operator=(const Uncopyable&);
};

// HomeForSale不用在声明拷贝构造函数和拷贝赋值运算符
class HomeForSale : private Uncopyable{
...
}
```

C++11 更好的方法是将其定义为 delete 的：

```cpp
class Uncopyable {
public:
    Uncopyable(const Uncopyable&) = delete;
    Uncopyable& operator=(const Uncopyable&) = delete;
};
```

## 条款 07：为多态基类声明 virtual 析构函数

C++ 当中派生类对象由一个基类指针删除，并且该基类是非 virtual 析构函数，那么会导致实际执行的时候派生类对象的资源并没有销毁，合理的做法是给基类添加一个 virtual 析构函数：

```cpp
class TimeKeeper {
public:
	TimeKeeper();
	virtual ~TimeKeeper();
};
TimeKeeper *ptk = getTimeKeeper(); // 工厂函数返回TimeKeeper的派生类成员对象
...
delete ptk; // 不会造成内存泄漏
```

对于一个类来说，如果没有任何的 virtual 函数，通常意味着它不会被用作基类，而令它的析构函数为 virtual，会导致类对象的空间增加（需要存放虚函数指针）。

如果想有一个抽象基类，但是没有任何合适的纯虚函数，可以给基类添加一个 virtual 虚函数，这样也不用担心析构的问题：

```cpp
class AWOV {
public:
	virtual ~AWOV() = 0; // 声明纯虚析构函数
}
```

对于这个函数需要为析构函数提供一份额外的定义，否则链接器会出现报错。

```cpp
AWOV::~AWOV() {} // 纯虚析构函数的定义
```

## 条款 08：别让异常逃离析构函数

C++ 没有禁止析构函数抛出异常，如果一个对象在析构函数当中抛出异常，可能会导致程序未定义的行为，因此需要尽可能的避免在析构函数当中抛出异常。

但是如果析构函数必须执行某个动作，而这个动作可能会抛出异常，考虑用下面 3 种方法：

```cpp
class DBConn {  
public:  
    ...  
    ~DBConn() {  
        db.close();    // 该函数可能会抛出异常  
    }  
  
private:  
    DBConnection db;  
};
```

- 1. 调用 abort 强制程序结束

```cpp
DBConn::~DBConn() {
    try { db.close(); }
    catch (...) {
        // 记录运行日志，以便调试
        std::abort();
    }
}
```

- 2. 吞下异常，不作为，这种情况不推荐

```cpp
DBConn::~DBConn() {
    try { db.close(); }
    catch (...) {
        // 记录运行日志，以便调试
    }
}
```

- 3. 给用户提供接口，使得用户可以有机会处理 close 发生的异常，下面代码当中用户可以手动调用 close 来释放连接，客户可以来进行处理相应的异常行为：

```cpp
class DBConn {
public:
    ...
    void close() { // 供用户使用的新函数
        db.close();
        closed = true;
    }

    ~DBConn() {
        if (!closed) {
            try {
                db.close();
            }
            catch(...) {
                // 处理异常
            }
        }
    }

private:
    DBConnection db;
    bool closed;
};
```

## 条款 09：绝不在构造和析构过程中调用 virtual 函数

在构造函数和析构函数当中调用虚函数，并不会表现出来多态性，例如下面代码：

```cpp
#include <iostream>
using namespace std;

class Transaction {
public:
    Transaction() {
        logTransaction();
    }
    virtual void logTransaction() {
        std::cout << "Transaction::Log!" << std::endl;
    }
};

class BuyTransaction : public Transaction {
public:    
    virtual void logTransaction() {
        std::cout << "BuyTransaction::Log!" << std::endl;
    }
};

int main() {
    BuyTransaction buy;
}
```

这里可以看到想通过基类的构造函数中，调用不同版本的 logTransaction() 从而不同的交易产生不同的 log 日志，但是构造函数中不会使用多态，上面创建的 buy 对象，只会打印输出基类的日志，而不是派生类的日志。

在派生类对象在构造基类对象期间，对象的类型是基类，而不是派生类，虚函数以及运行期类型信息（RTTI）会被绑定为基类，这通常不符合我们的预期。

如果想在 Transaction 继承体系下对象被创建的时候，会调用适当版本的 LogTransaction 函数，推荐的做法是将它改为 non-virtual 函数，然后要求派生类构造函数传递相应的信息给基类的构造函数，这样构造函数就可以安全的调用 non-virtual 函数：

```cpp
class Transaction {
public:
    explicit Transaction(const std::string& logInfo);
    void LogTransaction(const std::string& logInfo) const;
    ...
};

Transaction::Transaction(const std::string& logInfo) {
    LogTransaction(logInfo);                           // 更改为了非虚函数调用
}

class BuyTransaction : public Transaction {
public:
    BuyTransaction(...)
        : Transaction(CreateLogString(...)) { ... }    // 将信息传递给基类构造函数
    ...

private:
    static std::string CreateLogString(...);
}
```

这里使用了 static 的 CreateLogString 函数，使用辅助函数创建一个值传递给基类函数更加的方便、可读。而 static 可以确保在 BuyTransaction 当中不会使用处于未定义状态的成员变量。
## 条款 10：令 operator=返回一个 reference to \*this

连锁赋值方式：

```cpp
int x, y, z;
x = y = z = 1;
```

上面的连锁赋值被解析为：

```cpp
x = (y = (z = 1));
```

为了实现“连锁赋值”，赋值操作符必须返回一个指向操作符的左侧实参的引用：

```cpp
class Widge {
    public:
    ...
	Widge& operator=(const Widge& rhs){				//返回类型是个reference，指向当前对象
		...
		return *this;								//返回左侧对象
    }
    ...
};
```

它可以适用于下面这种：

```cpp
class Widge {
    public:
    ...
	Widge& operator+=(const Widge& rhs){	//这个协议适用于+=， -=， *= 等
		...
		return *this;								
    }
    Widge& operator=(int rhs){				//此函数也适用，即使此操作符的参数类型不符协定
		...
		return *this;							
    }
    ...
};
```

但是这种并没有强制，不采纳也是可以通过编译的，这里只是推荐这样做，即为赋值操作符返回一个指向 \*this 的引用。

## 条款 11：在 operator= 中处理“自我赋值”

“自我赋值”发生在对象被赋值给自己时：`w = w`，一般不会直接写这样的代码，但是一些场景下，可能会存在潜在的赋值：`a[i] = a[j]`，当 `i` 和 `j` 相等时会出现潜在的赋值，或者 `*px = *py`，当其指向同一个对象时，也是一种潜在的赋值。

这样的赋值会出现潜在的问题：

```cpp
Widget& operator+=(const Widget& rhs) {
    delete pRes;                          // 删除当前持有的资源
    pRes = new Resource(*rhs.pRes);       // 复制传入的资源
    return *this;
}
```

如果创建的两个指针指向同一个对象，在进行自我赋值的时候，有可能先删除了所分配的地址空间，这样的话，右边不可以赋值，出现错误。

一种解决办法是：进行一个证同测试，看是否是自我赋值。

```cpp
Widget& operator+=(const Widget& rhs) {
	if (this = &rhs) return *this;        // 测试是否是自我赋值
    delete pb;                            // 删除当前持有的资源
    pb = new Bitmap(*rhs.pb);           // 复制传入的资源
    return *this;
}
```

另一种方法：只关注异常安全，不考虑自我赋值，只要在赋值 pb 之前别删除 pb 即可.

```cpp
Widget& operator+=(const Widget& rhs) {
	Bitmap *pOrig = pb;        // 记住原先的pb
    pb = new Bitmap(*rhs.pb);  // 令pb指向*pb的一个副本
    delete pOrig;              // 删除原先的pb
    return *this;
}
```

也可以采用 copy and swap 手法：

```cpp
Widget& operator+=(const Widget& rhs) {
	Widget temp(rhs);  // 将rhs数据制作一份副本
	swap(*this, temp); // 交换this和rhs的数据
    return *this;
}
```

也可以通过传值的方式来实现交换：

```cpp
Widget& operator+=(Widget rhs) { // 传的参数为值，rhs是原来对象的副本
	swap(*this, rhs); // 交换this和rhs的数据
    return *this;
}
```

## 条款 12：复制对象时勿忘其每一个成分

在实现拷贝函数（包括拷贝构造函数和拷贝赋值运算符）的时候，不要忘记需要复制每一个成员变量，如果有变量没有复制，一般编译器不会检测出来，而这可能会导致隐患。

在继承体系下，派生类的继承基类的成员很可能会被忘记去手动复制，由于基类成员变量往往是 private，派生类无法直接返回，这个时候应该调用基类对象的拷贝函数从而对派生类当中基类成员进行复制：

```cpp
class PriorityCustomer : public Customer {
public:
    PriorityCustomer(const PriorityCustomer& rhs);
    PriorityCustomer& operator=(const PriorityCustomer& rhs);
    ...

private:
    int priority;
}

PriorityCustomer::PriorityCustomer(const PriorityCustomer& rhs)
    : Customer(rhs),                // 调用基类的拷贝构造函数
      priority(rhs.priority) {
    ...
}

PriorityCustomer::PriorityCustomer& operator=(const PriorityCustomer& rhs) {
    Customer::operator=(rhs);       // 调用基类的拷贝赋值运算符
    priority = rhs.priority;
    return *this;
}
```

**拷贝赋值运算符和拷贝构造函数互相调用也是不可以的，它们的目的不同，如果想简化代码，可以实现一个新的成员函数来供二者使用。**

---
# 3 资源管理
## 条款 13：以对象管理资源

使用 new 和 delete 管理资源，可能会导致潜在的内存泄漏问题：

```cpp
void f() {
	Invertment *p = createInvestment(); // 调用工厂函数，返回继承体系下的派生类对象
	...
	delete p; // 释放p指向的对象
}
```

上面代码中 ... 可能会存在潜在的抛出异常问题，如果提前抛出异常，delete p 就无法执行，就会导致内存泄漏。

更好的办法是采用 RAII（资源获取即初始化）的思想：
- 获得资源立刻放到管理对象当中；
- 管理对象自动调用析构函数确保资源被释放；

可以使用 C++11 的智能指针 `std::unique_ptr` 和 `std::shared_ptr` 来实现上面的目的，这里就不介绍了。

## 条款 14：在资源管理类中小心 copying 行为

对于一个采用 RAII 思想的类对象来说，如果发生拷贝，需要怎么做？

1. **禁止复制**：参考条款 06
2. **采用引用计数**：类似于 `std::shared_ptr` 的思路，当引用计数为 0 的时候再去释放资源，可以在管理类当中直接使用 `std::shared_ptr` 来接管资源。
3. **深拷贝**：采用深拷贝的方式来复制所有的资源，如果类中有指针成员，不仅需要拷贝指针变量，还需要拷贝指针所指向的数据。
4. **转移所有权**：采用 `std::unique_ptr` 的思路，将一个对象的所有权转移到另外一个对象当中。

## 条款 15：在资源管理类中提供对原始资源的访问

C++ 的智能指针都提供的 `get` 函数来获取原始指针，这样是为了在某些场景下能够正确的交互，比如在 C 语言的 API 当中，只能使用原始指针，而不能传递智能指针作为参数。

如果我们自己设计资源管理类的时候，也需要考虑为用户提供原始资源的方法，有两种思路：**显示转换**和**隐式转换**。

```cpp
class Font { // 某个RAII类
public:
	FontHandle get() { return f; }            // 显示转换
	operator FontHandle() const { return f; } // 隐式转换
...
private:
	FontHandle f; // 原始资源
}
```

隐式转换可能会存在潜在的隐患：

```cpp
Font f1(getFont());
...
FontHandle f2 = f1; // 这里本意想进行拷贝，但是实际将f1隐式转换为底层的FontHandle再进行复制
```

一般而言显式转换比较安全，但隐式转换对客户比较方便。

## 条款 16：成对使用 new 和 delete 时要采取相同形式

这条比较简单，而且应该是老生常谈的问题了，如果你调用 new 时使用 []，你必须在对应调用 delete 时也使用 []。如果你调用 new 时没有使用 []，那么也不该在对应调用 delete 时使用 []。

```cpp
std::string* sp1 = new std::string;
std::string* sp2 = new string[100];
...
delete sp1;   // 删除一个对象
delete[] sp2; // 删除一组对象
```

使用 typedef 的时候需要注意 delete 进行匹配：

```cpp
typedef std::string AddressLines[4]; // string数组，大小为4
std::string* p = new AddressLines; // 实际等于new string[4];

delete p;   // 不正确
delete []p; // 正确
```

## 条款 17：以独立语句将 newed 对象置入智能指针

使用 `std::shared_ptr` 可能存在潜在的内存泄漏，这种情况一般发生在参数传参的时候，使用 new 来创建了 `std::shared_ptr` 对象，同时参数传递的时候，还有另外一个函数：

```cpp
// 根据优先级处理Widge
void processWidget(std::shared_ptr<Widget> spw, int priority);
// 另外一个计算优先级的函数
int computePriority();

int main() {
	processWidget(std::shared_ptr<Widget>(new Widget),  //潜在的资源泄漏！
              computePriority());
}
```

上面可能的内存泄漏是因为，C++ 没有规定参数的执行顺序，这里需要先计算出实参的内容，然后再调用这个函数，可能编译器的执行顺序如下：

- 1. 执行 `new Widget`
- 2. 执行 `computePriority`
- 3. 运行 `std::shared_ptr` 构造函数

如果 computePriority 函数中抛出异常了，这里因为没有调用 `std::shared_ptr` 构造函数，但是执行了 new 语句，也就会导致内存泄漏。

解决办法是将 new 语句创建的智能指针单独写在一个语句当中：

```cpp
std::shared_ptr<Widget> sp(new Widget);
processWidget(sp, computePriority());
```

C++11 更好的办法是使用 `std::make_shared` ：

```cpp
processWidget(std::make_shared<Widget>(), //没有潜在的资源泄漏
		  computePriority());
```
