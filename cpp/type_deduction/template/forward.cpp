#include <iostream>
using namespace std;

template <typename T>
void f(T&& param) {}  // param现在是一个通用引用类型

int main() {
  int x = 27;          // 如之前一样
  const int cx = x;    // 如之前一样
  const int& rx = cx;  // 如之前一样

  f(x);  // x是左值，所以T是int&，param类型也是int&
  f(cx);  // cx是左值，所以T是const int&，param类型也是const int&
  f(rx);  // rx是左值，所以T是const int&，param类型也是const int&
  f(27);  // 27是右值，所以T是int，param类型就是int&&
}