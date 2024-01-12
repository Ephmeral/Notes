#include <iostream>
using namespace std;

int main() {
  int x = 27;
  auto y = x;         // 3.ParamType不是指针也不是引用，y为int类型
  const auto cx = x;  // 3.ParamType不是指针也不是引用，cx为const int类型
  const auto& rx = cx;// 1. ParamType是引用，当不是万能引用，rx为const int&类型

  // 2. ParamType是万能引用
  auto&& uref1 = x;   // x是int左值，所以uref1类型为int&
  auto&& uref2 = cx;  // cx是const int左值，所以uref2类型为const int&
  auto&& uref3 = 27;  // 27是int右值，所以uref3类型为int&&
}