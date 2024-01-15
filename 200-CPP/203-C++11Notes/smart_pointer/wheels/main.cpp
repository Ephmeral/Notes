#include <iostream>

#include "UniquePtr.h"

class Base {
public:
    Base() = default;
    Base(int x) : _x(x) {}

    virtual void vfunc() {
        std::cout << "Base virtual function" << std::endl;
    }

private:
    int _x;
};

int main() {
    unique_ptr<int> ptr(new int(100));

    std::cout << *ptr << std::endl;
    std::cout << ptr.get() << std::endl;

    ptr.reset(new int(50));

    std::cout << *ptr << std::endl;
    std::cout << ptr.get() << std::endl;

    unique_ptr<Base> pBase(new Base(100));
    pBase->vfunc();

    // std::make_unique<int> up;

    // auto sp = make_shared<int>(100);

    return 0;
}