// ambiguity.cpp
#include <iostream>
using namespace std;

void f(int x) { std::cout << "call f(int x)" << std::endl; }

void f(bool x) { std::cout << "call f(bool x)" << std::endl; }

void f(char* x) { std::cout << "call f(char* x)" << std::endl; }

int main() {
  f(0);                              // call f(int x)
  // f(NULL);                        // 编译失败
  f(reinterpret_cast<char*>(NULL));  // call f(char* x)
  f(nullptr);                        // call f(char* x)
}