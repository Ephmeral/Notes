# C++笔记 | 运行时类型识别（RTTI）
C++ 的 RTTI（Run-Time Type Information）是一种运行时类型信息机制，用于在程序运行时获取对象的类型信息。RTTI 主要包括两个关键字：typeid 和 dynamic_cast。

- typeid 运算符，用于返回表达式的类型。  
- dynamic_cast 运算符，用于将基类的指针或引用安全地转换成派生类的指针或引用。

一般来说动态类型值得是某个基类指针或引用，指向一个派生类对象，且基类中有虚函数，此时会绑定对象的动态类型。

## dynamic_cast

基本用法：

```cpp
dynamic_cast<type*>(e)  // e必须是有效的指针
dynamic_cast<type&>(e)  // e必须是个左值
dynamic_cast<type&&>(e) // e不能是左值
```

- type 是类类型，通常应该有虚函数
- e 必须满足以下三者之一：
	- e 是 type 的公有派生类
	- e 是 type 的公有基类
	- e 就是 type 类型
- 指针失败返回 0，引用失败返回 bad_cast 异常

```cpp
// rtti/dynamic_cast.cpp
#include <iostream>
using namespace std;

class Base {
public:
    Base(int _i) : i(_i) {}
    virtual void print() {
        cout << " i = " << i << endl;
    }
private:
    int i;
};

class Derive : public Base {
public:
    Derive(int i) : Base(i) {}
};

int main() {
    // 第一种情况，Base指针指向Derive对象，可以将Base指针转为Derive指针
    Derive derive(10);
    Base *bp = &derive;
    if (Derive *dp = dynamic_cast<Derive*>(bp)) { // 可以成功转型
        cout << "dynamic cast succeed~ ";
        dp->print();
    } else {
        cout << "dynamic cast failed!" << endl;
    }

    // 第二种情况，Base指针指向Base对象，不能将Base指针转为Derive指针
    Base base(15);
    bp = &base;
    if (Derive *dp = dynamic_cast<Derive*>(bp)) { // 转型失败
        cout << "dynamic cast succeed~ ";
        dp->print();
    } else {
        cout << "dynamic cast failed!" << endl;
    }

    // 第三种情况，Derive指针指向Derive对象，可以将Derive指针转为Base指针
    Derive *dp = new Derive(15);
    if (Base *bp = dynamic_cast<Base*>(dp)) { // 转型成功
        cout << "dynamic cast succeed~ ";
        bp->print();
    } else {
        cout << "dynamic cast failed!" << endl;
    }
    // 以下代码不能运行，不能将Derive指针指向Base对象
    // Derive *dp = new Base(20);
    return 0;
}
/*
$ g++ dynamic_cast.cpp && ./a.out
dynamic cast succeed~  i = 10
dynamic cast failed!
dynamic cast succeed~  i = 15
*/
```

 小结：
 - 想通过 dynamic_cast 进行动态的转型，基类必须有虚函数；
 - 基类指针必须指向派生类对象，才可以将其转为派生类指针；
- 指向派生类对象的派生类指针，自然可以转为基类指针；
- 不能将派生类指针指向基类对象；

如果是引用类型，因为不存在空引用，返回的是 bad_cast 异常，可以用下面方法进行捕获：

```cpp
void f(const Base &b) {
	try {
		const Derive &d = dynamic_cast<const Derive&>(b);
		// ...
	} catch (bad_cast) {
		// 处理类型转换失败的情况
	}
}
```

练习：什么情况下应该用 dynamic_cast 替代虚函数

如果我们需要在派生类中增加新的成员函数 f，但又无法取得基类的源代码，因而无法在基类中增加相应的虚函数，这时，可以在派生类中增加非虚成员函数。但这样一来，就无法用基类指针调用函数 f。如果在程序中需要通过基类指针（如使用该继承层次的某个类中所包含的指向基类对象的指针数据成员 p）来调用 f，则必须使用 dynamic_cast 将 p 转换为只想派生类的指针，才能调用 f。也就是说，如果无法为基类增加虚函数，就可以使用 dynamic_cast 代替虚函数。

## typeid

为 RTTI 提供的第二个运算符是 typeid 运算符（typeid operator），它允许程序向表达式提问：你的对象是什么类型？

```cpp
typeid(e) 
```

- e 可以是任意表达式或类类型；
- typeid 返回值是对一个常量对象的引用，该对象的类型是标准库类型 type_info 或者 type_info 的公有派生类型；
- e 如果是引用，typeid 会返回该引用所引用的对象类型；
- typeid 作用在数组上时，返回的是数组类型，而不是指针类型；
- 运算对象不是类类型，或者不包含虚函数的类，指示的是静态类型；
- 如果是定义了一个虚函数的类型，typeid 的结果运行时才会求得；

```cpp
// rtti/typeid.cpp
#include <iostream>
using namespace std;

class Base {
public:
    Base(int _i) : i(_i) {}
    virtual void print() {
        cout << " i = " << i << endl;
    }
private:
    int i;
};

class Derive : public Base {
public:
    Derive(int i) : Base(i) {}
};

int main() {
    Derive *dp = new Derive(10);
    Base *bp = dp; // 两个指针都指向Derive对象

    if (typeid(*bp) == typeid(*dp)) {
        cout << "bp和dp指向同一个类型的对象" << endl;
    }

    if (typeid(*bp) == typeid(Derive)) {
        cout << "bp指向的是Derive对象" << endl;
    }
    return 0;
}
/*
$ g++ rtti2.cpp && ./a.out
bp和dp指向同一个类型的对象
bp指向的是Derive对象
*/
```

注：
- typeid 应该作用于对象，而不是指针本身，因此需要使用 `*bp`
- 只有当类型有虚函数的时候，编译器才会在运行时对表达式进行求值；
- 如果类型没有虚函数，typeid 返回的是静态类型，编译器无须求值也知道表达式的静态类型；
- 如果指针 p 指向的类型**没有虚函数**，则计算 `typeid(*p)` 的时候 `*p` 可以不是一个有效的对象；
- 如果指针 p 指向的类型**有虚函数**， `*p` 会在运行时求值，如果 p 是一个空指针，会抛出 bad_typeid 的异常；

下面是空指针运算 `*p` 抛异常的代码片段：

```cpp
// data_types/rtti2.cpp
Base *p = nullptr;
if (typeid(*p) == typeid(Base)) {
	cout << "p指向的对象是Base类型" << endl;
} else {
	cout << "p指向的对象不是Base类型" << endl;
}
/*
$ g++ rtti2.cpp && ./a.out
terminate called after throwing an instance of 'std::bad_typeid'
  what():  std::bad_typeid
Aborted (core dumped)
*/
```

## 使用 RTTI

现有一个基类和一个派生类，我们需要为其实现相等运算符，一种考虑的方法是定义一套虚函数，然后各自在判断是否相等。

这样的问题是因为虚函数中基类和派生类必须有相同的形参，如果我们想定义一个虚函数 equal，则该函数的形参必须是基类的引用。此时，equal 函数将只能使用基类的成员，而不能比较派生类独有的成员。

下面是解决方法，可以利用 typeid 先判断类型是否相同，然后再调用各自版本的 equal 函数：
```cpp
// data_types/rtti_example.cpp
#include <iostream>
using namespace std;

class Base {
    friend bool operator==(const Base&, const Base&);
public:
    Base(int _i, int _j) : i(_i), j(_j) {}
protected:
    virtual bool equal(const Base&) const;
    int i;
    int j;
};

class Derive : public Base {
public:
    Derive(int _i, int _j, int _k) : Base(_i, _j), k(_k) {}
protected:
    bool equal(const Base&) const;
private:
    int k;
};

bool operator==(const Base &lhs, const Base &rhs) {
    // 如果typeid不相等返回false；否则调用equal()
    return typeid(lhs) == typeid(rhs) && lhs.equal(rhs);
}

bool Derive::equal(const Base &rhs) const {
    cout << "Derive::equal()" << endl;
    // 我们清楚这两个类型是相等的，所以转换过程不会抛出异常
    auto r = dynamic_cast<const Derive&>(rhs);
    // 执行比较两个Derive对象的操作并返回
    return this->i == r.i && this->j == r.j && this->k == r.k;
}

bool Base::equal(const Base &rhs) const {
    cout << "Base::equal()" << endl;
    // 执行比较Base对象的操作
    return this->i == rhs.i && this->j == rhs.j;
}

int main() {
    // 第一种情况：两个基类指针进行判断
    Base *bp1 = new Base(10, 20);
    Base *bp2 = new Base(10, 20);
    if (*bp1 == *bp2) {
        cout << "bp1和bp2相等" << endl;
    }
    // 第二种情况：一个基类指针和一个派生类指针，都指向派生类对象
    Base *bp = new Derive(10, 20, 30);
    Derive *dp = new Derive(10, 20, 30);
    if (*bp == *dp) {
        cout << "bp和dp指向的对象相同" << endl;
    }
    // 第三种情况：两个指针分别指向基类对象和派生类对象，他两不能直接判断
    Base *bp3 = new Base(10, 20);
    Base *bp4 = new Derive(10, 20, 30);
    if (*bp3 == *bp4) {
        cout << "bp3和bp4指向的对象相同" << endl;
    } else {
        cout << "bp3和bp4指向的对象不相同" << endl;
    }
    return 0;
}
/*
$ g++ rtti_example.cpp && ./a.out
Base::equal()
bp1和bp2相等
Derive::equal()
bp和dp指向的对象相同
bp3和bp4指向的对象不相同
*/
```

可以看出相同动态类型在判断的时候会调用各自版本的 equal 函数，而类型不相同的 typeid 判断相等直接返回 false 了

## type_info 类

type_info 类不同编译器定义各有不同，C++ 标准规定必须定义在 typeinfo 头文件中，且必须支持以下操作：

| 函数名     | 描述                                                         |
|------------|--------------------------------------------------------------|
| t1 == t2   | 如果 type_info 对象 t1 和 t2 表示同一种类型，返回 true；否则返回 false |
| t1 != t2   | 如果 type_info 对象 t1 和 t2 表示不同的类型，返回 true；否则返回 false |
| t.name()   | 返回一个 C 风格字符串，表示类型名字的可打印形式。类型名字的生成方式因系统而异 |
| t1.before(t2) | 返回一个 bool 值，表示 t1 是否位于 t2 之前。before 所采用的顺序关系是依赖于编译器的 |

- type_info 类无默认构造函数，拷贝/移动构造函数和赋值运算符都是定义为删除的，因此无法定义或拷贝 type_info 类型的对象，也不能为其赋值；
- type_info 唯一作用是使用 typeid 运算符；
- type_info 类的 `name()` 成员函数返回一个 C 风格字符串，表示对象的类型名字，但是具体返回是什么由编译器决定；

下面是一个简单的示例（编译器 g++ (Ubuntu 11.3.0-1ubuntu1~22.04.1) 11.3.0）：

```cpp
// data_types/typeinfo.cpp
#include <iostream>
using namespace std;

class Base {
};

class Derive : public Base {
};

int main() {
    int arr[10];
    int *arrp = new int[10];
    Derive d;
    Base *p = &d;

    cout << typeid(42).name() << ", "
         << typeid(42.0).name() << ", "
         << typeid('a').name() << ", "
         << typeid("string").name() << ", "
         << typeid(std::string).name() << ", "
         << typeid(arr).name() << ", "
         << typeid(arrp).name() << ", "
         << typeid(d).name() << ", "
         << typeid(p).name() << ", "
         << typeid(*p).name() << endl;
    return 0;
}
/*
$ g++ typeinfo.cpp && ./a.out
i, d, c, A7_c, NSt7__cxx1112basic_stringIcSt11char_traitsIcESaIcEEE, A10_i, Pi, 6Derive, P4Base, 4Base
*/
```

## 补充：静态类型/动态类型

C++ 中的静态类型指的是：变量的数据类型在编译时、程序执行前就确定的。C++ 本身时一种静态类型语言，它在编译期间会确定数据类型并进行检查。

C++ 的动态类型：一般指的是对象所指的类型，在运行期间才能确定的，一般只有指针或者引用才用动态类型的说法。

```cpp
#include <iostream>
using namespace std;

class Base { 
};
class Derive : public Base { 
};

int main() {
	int a = 10;       // 静态类型是int，不存在动态类型的说法
	double b = 100.0; // 静态类型是double，不存在动态类型的说法
	Base base;        // 静态类型是Base
	Derive derive;    // 静态类型是Derive
	Base *pbase;      // 定义的时候是Base*，所以静态类型是Base
	Base *pbase2 = new Derive(); // 静态类型依旧是Base*，动态类型才是Derive*
	return 0;
}
```