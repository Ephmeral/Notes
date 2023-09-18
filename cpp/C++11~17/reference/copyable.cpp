// copyable.cpp
#include <iostream>
using namespace std;

struct Copyable {
    Copyable() {}
    Copyable(const Copyable &o) {
        cout << "Copied" << endl;
    }
};

Copyable ReturnRvalue() { 
    return Copyable(); 
}

void AcceptVal(Copyable) {}

void AcceptRef(const Copyable & ) {}

int main() {
    cout << "Pass by value: " << endl;
    AcceptVal(ReturnRvalue()); // 临时值被拷贝传入
    cout << "Pass by reference: " << endl;
    AcceptRef(ReturnRvalue()); // 临时值被作为引用传递
}
// g++ 3-3-5.cpp -fno-elide-constructors