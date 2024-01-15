#include <iostream>
using namespace std;

decltype(auto) f1() {
	int x = 0;
	// ...
	return x; // 返回int
}

decltype(auto) f2() {
	int x = 0;
	// ...
	return (x); // 返回int&，但是这样引用了一个局部变量，会产生未定义行为
}

int main() {
  int i;
  int&& f();
  auto x1a = i;                 // x1a推导类型为int
  decltype(auto) x1d = i;       // x1d推导类型为int

  auto x2a = (i);               // x2a推导类型为int
  decltype(auto) x2d = (i);     // x2d推导类型为int&

  auto x3a = f();               // x3a推导类型为int
  decltype(auto) x3d = f();     // x3d推导类型为int&&

  auto x4a = {1, 2};            // x4a推导类型为std::initializer_list<int>
  decltype(auto) x4d = {1, 2};  // 编译失败, {1, 2}不是表达式

  auto* x5a = &i;               // x5a推导类型为int*
  decltype(auto)* x5d = &i;     // 编译失败，decltype(auto)必须单独声明

  auto&& x = f1(); // int&&
  auto&& y = f2(); // int&
}