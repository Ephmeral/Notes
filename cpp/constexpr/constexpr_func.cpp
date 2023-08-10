// constexpr_func.cpp
#include <iostream>
using namespace std;

constexpr int max_unsigned_char() { return 0xff; }

constexpr int square(int x) { return x * x; }

constexpr int abs(int x) { return x > 0 ? x : -x; }

/************反例**********/
// 下面的几个函数使用g++ constexpr_func.cpp -std=c++11 则无法通过编译
// 返回类型为void
constexpr void foo() {} 

// ++x不是常量表达式 
constexpr int next(int x) { return ++x; }

int g() { return 42; }

// g() 不是常量表达式
constexpr int f() { return g(); }
// 只有声明没有定义
constexpr int max_unsigned_char2();

enum {
  max_uchar = max_unsigned_char2()
};

// 多条语句
constexpr int abs2(int x) {
  if (x > 0) {
    return x;
  } else {
    return -x;
  }
}

// 多条语句
constexpr int sum(int x) {
  int result = 0;
  while (x > 0) {
    result += x--;
  }
  return result;
}

int main() {
  char buffer1[max_unsigned_char()] = {0};
  char buffer2[square(5)] = {0};
  char buffer3[abs(-8)] = {0};
}