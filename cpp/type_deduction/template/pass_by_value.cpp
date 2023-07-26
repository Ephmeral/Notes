#include <iostream>
using namespace std;

template <typename T>
void f(T param) {}  // 以传值的方式处理param

int main() {
  int x = 27;          // 如之前一样
  const int cx = x;    // 如之前一样
  const int& rx = cx;  // 如之前一样

  f(x);   // T和param的类型都是int
  f(cx);  // T和param的类型都是int
  f(rx);  // T和param的类型都是int
}