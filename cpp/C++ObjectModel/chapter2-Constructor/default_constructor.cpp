#include <iostream>
using namespace std;

class Foo {
public:
    Foo() {}
    Foo(int) {}
};

class Foo2 {
public:
    Foo2() {}
    Foo2(int) {}
};

class Foo3 {
public:
    Foo3() {}
    Foo3(int) {}
};

class Bar {
public:
    Bar() {
        str = 0;
    }
    Foo foo;
  	Foo2 foo2;
  	Foo3 foo3;
    char *str;
};

int main() {
    Bar bar;
   
}