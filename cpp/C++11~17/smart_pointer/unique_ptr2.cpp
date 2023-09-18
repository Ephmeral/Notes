// unique_ptr2.cpp
#include <cassert>
#include <iostream>
#include <memory>

struct Foo {
    Foo() { std::cout << "Foo\n"; }
    ~Foo() { std::cout << "~Foo\n"; }
};

struct D {
    void operator() (Foo* p) {
        std::cout << "Calling delete for Foo object... \n";
        delete p;
    }
};

int main() {
    // 创建一个新的unique_ptr对象，指向Foo
    std::unique_ptr<Foo> up1(new Foo()); 
    // 释放up1的所有权，返回Foo对象指针
    Foo* fp = up1.release();
    assert(up1.get() == nullptr);
    
    std::cout << "==========================" << std::endl;

    std::cout << "Creating new Foo...\n";
    std::unique_ptr<Foo, D> up(new Foo(), D());  // up 占有 Foo 指针（删除器 D ）
 
    std::cout << "Replace owned Foo with a new Foo...\n";
    up.reset(new Foo());  // 替换up2指向的对象，并调用旧者的删除器
 
    std::cout << "Release and delete the owned Foo...\n";
    up.reset(nullptr);          // 将up2置空
}