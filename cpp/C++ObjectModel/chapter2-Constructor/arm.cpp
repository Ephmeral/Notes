#include <iostream>
using namespace std;

class Foo {
public:
    int val;
    Foo *pnext;
};

void foo_bar() {
    Foo bar;
    if (bar.val || bar.pnext) {

    }
    std::cout << "bar.val = " << bar.val << std::endl
              << "bar.pnext = " << bar.pnext << std::endl;
}

int main() {
    foo_bar();
}