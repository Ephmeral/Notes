#include <iostream>
using namespace std;

void someFunc(int, double) {}  // someFunc是一个函数，类型是void(int, double)

template <typename T>
void f1(T param) {}  // 传值给f1

template <typename T>
void f2(T& param) {}  // 传引用给f2

int main() {
  f1(someFunc);  // param被推导为指向函数的指针，类型是void(*)(int, double)
  f2(someFunc);  // param被推导为指向函数的引用，类型是void(&)(int, double)
}