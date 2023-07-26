#include <iostream>
using namespace std;

template <typename T>
void f(T& param) {
  std::cout << param << std::endl;
}

int main() {
  int x = 27;         // x 是 int
  const int cx = x;   // cx 是 const int
  const int& rx = x;  // rx 是指向作为 const int 的 x 的引用

  // 推导结果
  f(x);   // T是int，param的类型是int&
  f(cx);  // T是const int，param的类型是const int&
  f(rx);  // T是const int，param的类型是const int&
}
