// func_return_value.cpp
#include <iostream>
using namespace std;

// 返回值推导为int
auto sum(int x, int y) { return x + y; }

auto add(long x, long y) {
  if (x < 0) {
    return 0;  // 返回类型为int，改成0L即可
  }
  return x + y;  // 返回类型为long
}

auto createInitList() { 
    return {1, 2, 3};  //错误！不能推导{ 1, 2, 3 }的类型
}

int main() {
  int x = 10, y = 20;
  auto z = sum(x, y);
}