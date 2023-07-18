// shared_prt6.cpp
#include <iostream>
#include <memory>
using namespace std;

class Foo {
public:
    Foo() {
        std::cout << "Foo默认构造函数..." << std::endl;
    }
    Foo(int _x) : x(_x) {
        std::cout << "Foo(int x)构造函数" << std::endl;
    }
    ~Foo() {}

private:
    int x;
};

int main() {
    auto sp = std::make_shared<int>(10);
    if (sp.unique()) {
        std::cout << "sp引用计数为1" << std::endl;
    }
    std::shared_ptr<int> ssp = sp;
    std::cout << "sp use_count = " << sp.use_count() << std::endl;

    auto fp1 = std::make_shared<Foo>();    // 调用Foo默认构造函数
    auto fp2 = std::make_shared<Foo>(10);  // 调用Foo(int x)构造函数
    return 0;
}
/*
$ g++ shared_ptr6.cpp -o shared_ptr && ./shared_ptr 
sp引用计数为1
sp use_count = 2
Foo默认构造函数...
Foo(int x)构造函数
*/