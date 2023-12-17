#include <iostream>
using namespace std;

class X { public: int i; };
class A : public virtual X { public: int j; };
class B : public virtual X { public: double d; };
class C : public A, public B { public: int k; };

// 编译器无法在编译的时候，解析出pa->X::i的位置
void foo(A* pa) {
    pa->i = 1024;
}

int main() {
    foo(new A);
    foo(new C);
}