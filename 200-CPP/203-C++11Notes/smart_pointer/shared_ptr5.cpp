// shared_ptr5.cpp
#include <cstdio>
#include <iostream>
#include <memory>

struct Foo {
    Foo() {
        std::cout << "Foo: constructor\n";
    }
    ~Foo() {
        std::cout << "Foo: destructor\n";
    }
};

int main() {
    std::shared_ptr<Foo> sp(new Foo);
    if (sp.unique()) {
        std::cout << "sp 的引用计数为1" << std::endl;
    } else{
        std::cout << "sp 的引用计数不为1" << std::endl;
    }
    std::cout << "sp ref count = " << sp.use_count() << std::endl;

    std::shared_ptr<Foo> sp2(sp); // 拷贝构造，引用计数+1
    if (sp.unique()) {
        std::cout << "sp 的引用计数为1" << std::endl;
    } else{
        std::cout << "sp 的引用计数不为1" << std::endl;
    }
    std::cout << "sp ref count = " << sp.use_count() << std::endl;
    return 0;
}
/*
$ g++ shared_ptr5.cpp -o shared_ptr && ./shared_ptr 
Foo: constructor
sp 的引用计数为1 
sp ref count = 1
sp 的引用计数不为1
sp ref count = 2
Foo: destructor
*/