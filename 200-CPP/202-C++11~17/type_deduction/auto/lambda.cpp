// lambda.cpp
#include <iostream>
#include <vector>
using namespace std;

int main() {
  auto sum = [](auto x, auto y) { return x + y; };

  auto z = sum(10, 20.5);
  std::cout << "z = " << z << std::endl;

  std::vector<int> v;
  auto resetV = [&v](const auto& newValue) { v = newValue; };  // C++14
  resetV({1, 2, 3});  // 错误！不能推导{ 1, 2, 3 }的类型
}