// shared_ptr2.cpp
#include <cstdio>
#include <iostream>
#include <memory>

class Foo {
public:
    Foo() { std::cout << "Foo...\n"; }
    ~Foo() { std::cout << "~Foo...\n"; }
    void someFunction() {
        printf("Some function, the addr = %p\n", this);
    }
};

int main() {
    std::shared_ptr<Foo> sp1(new Foo);
    Foo *foo = sp1.get();  // get方法获得原始指针
    foo->someFunction();
    sp1->someFunction();    // operator->
    (*sp1).someFunction();  // operator*
    return 0;
}
/*
$ g++ shared_ptr2.cpp -o shared_ptr && ./shared_ptr 
Foo...
Some function, the addr = 0x557ea5f79eb0
Some function, the addr = 0x557ea5f79eb0
Some function, the addr = 0x557ea5f79eb0
~Foo...
*/