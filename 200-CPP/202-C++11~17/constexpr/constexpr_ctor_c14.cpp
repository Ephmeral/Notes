#include <iostream>

class X {
 public:
  constexpr X() : x1(5) {}
  // 构造函数中可以有多条语句
  constexpr X(int i) : x1(0) {
    if (i > 0) {
      x1 = 5;
    } else {
      x1 = 8;
    }
  }
  constexpr void set(int i) { x1 = i; }
  constexpr int get() const { return x1; }

 private:
  int x1;
};

// 可以修改生命周期和常量表达式一样的变量
constexpr X make_x() {
  X x;
  x.set(42);
  return x;
}

int main() {
  constexpr X x1(-1);
  constexpr X x2 = make_x();
  constexpr int a1 = x1.get();
  constexpr int a2 = x2.get();
  std::cout << a1 << std::endl;  // 8
  std::cout << a2 << std::endl;  // 42
}
/*
$ g++ constexpr_ctor_c14.cpp -std=c++14 && ./a.out 
8
42
*/