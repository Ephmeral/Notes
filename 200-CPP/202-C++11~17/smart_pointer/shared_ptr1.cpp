// shared_ptr1.cpp
#include <memory>
#include <iostream>
 
class Foo {
public:
    Foo() { std::cout << "Foo...\n"; }
    ~Foo() { std::cout << "~Foo...\n"; }
};

// 函数对象，作为删除器
class D { 
public:
    void operator()(Foo* p) const {
        std::cout << "函数对象作为删除器...\n";
        delete p;
    }
};
 
int main()
{
    {
        std::cout << "默认构造函数，无任何参数\n";
        std::shared_ptr<Foo> sh1;
        std::cout << "=======================\n";
    }
 
    {
        std::cout << "构造函数，有参数\n";
        std::shared_ptr<Foo> sh2(new Foo); // 原始指针
        std::shared_ptr<Foo> sh3(sh2);     // 拷贝构造
        std::cout << "sh2 的引用计数为 " << sh2.use_count() << '\n';
        std::cout << "sh3 的引用计数为 " << sh3.use_count() << '\n';
        std::cout << "=======================\n";
    }
 
    {
        std::cout << "构造函数，有参数和删除器\n";
        std::shared_ptr<Foo> sh4(new Foo, D());
        std::shared_ptr<Foo> sh5(new Foo, [](auto p) {
           std::cout << "lambda函数作为删除器...\n";
           delete p;
        });
        std::cout << "=======================\n";
    }

    // {
    //     std::cout << "unique_ptr作为参数调用构造函数\n";
    //     std::unique_ptr<Foo> uq(new Foo);
    //     std::shared_ptr<Foo> sh6(std::move(uq));
    //     std::cout << "=======================\n";
    // }
}