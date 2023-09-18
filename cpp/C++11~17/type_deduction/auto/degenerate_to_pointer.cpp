#include <iostream>
using namespace std;

int main() {
  const char name[] = "R. N. Briggs";  // name的类型是const char[13]

  auto arr1 = name;   // arr1的类型是const char*
  auto& arr2 = name;  // arr2的类型是const char (&)[13]

  void someFunc(int, double);  // someFunc是一个函数，类型为void(int, double)

  auto func1 = someFunc;   // func1的类型是void (*)(int, double)
  auto& func2 = someFunc;  // func2的类型是void (&)(int, double)
}