#include <iostream>
using namespace std;

template <typename T>
void f(const T& param) {}  // param现在是reference-to-const

int main() {
  int x = 27;         // 如之前一样
  const int cx = x;   // 如之前一样
  const int& rx = x;  // 如之前一样

  f(x);   // T是int，param的类型是const int&
  f(cx);  // T是int，param的类型是const int&
  f(rx);  // T是int，param的类型是const int&
}