// main.cpp
#include <iostream>
#include <type_traits>
#include <typeinfo>

template <typename T>
void f(const T& param) {
  // 使用 decltype 获得param类型，带有引用修饰符
  using ReferenctParamType = decltype(param);
  // 去除引用修饰符，用来单独判断是否为const
  using ConstParamType =
      typename std::remove_reference<ReferenctParamType>::type;

  std::cout << std::boolalpha;
  std::cout << "T Is const: " << std::is_const<T>::value << std::endl;
  std::cout << "T Is is reference: " << std::is_reference<T>::value
            << std::endl;

  std::cout << "ParamType Is const: " << std::is_const<ConstParamType>::value
            << std::endl;
  std::cout << "ParamType is reference: "
            << std::is_reference<ReferenctParamType>::value << std::endl;
  std::cout << "======================================" << std::endl;
}

int main() {
  int x = 27;         // x 是 int
  const int cx = x;   // cx 是 const int
  const int& rx = x;  // rx 是指向作为 const int 的 x 的引用

  // 推导结果
  f(x);   // T是int，param的类型是const int&
  f(cx);  // T是int，param的类型是const int&
  f(rx);  // T是int，param的类型是const int&

  return 0;
}