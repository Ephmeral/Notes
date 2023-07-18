// shared_ptr4.cpp
#include <iostream>
#include <memory>

struct Foo {
    Foo(int n = 0) noexcept : bar(n) {
        std::cout << "Foo: constructor, bar = " << bar << '\n';
    }
    ~Foo() {
        std::cout << "Foo: destructor, bar = " << bar << '\n';
    }
    int getBar() const noexcept { return bar; }

private:
    int bar;
};

int main() {
    std::shared_ptr<Foo> sptr = std::make_shared<Foo>(1);
    std::cout << "The first Foo's bar is " << sptr->getBar() << "\n";

    // 重置，交与新的 Foo 实例
    // （此调用后将销毁旧实例）
    sptr.reset(new Foo);
    std::cout << "The second Foo's bar is " << sptr->getBar() << "\n";
    return 0;
}
/*
$ g++ shared_ptr4.cpp -o shared_ptr && ./shared_ptr 
Foo: constructor, bar = 1
The first Foo's bar is 1
Foo: constructor, bar = 0
Foo: destructor, bar = 1
The second Foo's bar is 0
Foo: destructor, bar = 0
*/