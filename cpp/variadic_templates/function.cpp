#include <iostream>
using namespace std;

void foo() {
    // do nothing
}

template<typename T, typename ...Args>
void foo(T first, Args ...args) {
    std::cout << first << std::endl;
    foo(args...);
}

int main()
{
  unsigned int x = 8;
  foo();            // foo()
  foo(1);           // foo<int>(int)
  foo(1, 11.7);     // foo<int,double>(int,double)
  foo(1, 11.7, x);  // foo<int,double,unsigned int>(int,double,unsigned int)
}