// decltype.cpp
#include <iostream>
using namespace std;

struct A {
  double x;
};
const A* a;

decltype(a->x) y;        // y 的类型是 double（其声明类型）
decltype((a->x)) z = y;  // z 的类型是 const double&（左值表达式）

int i = 0;
decltype(i) j;    // j的类型是int
decltype((i)) k;  // k的类型是int&

const int&& foo();
decltype(foo()) fn = 10;  // fn类型是 cont int&&

int main() {
  int i;
  int* j;
  int n[10];
  // i = 0，返回的是i，是左值，规则3，decltype(i=0)推导类型为int&
  decltype(i = 0);

  // 补充，逗号表达式，先计算左边的，再计算右边的，最后返回值是右边的
  // 0, i表达式返回的是i，是左值，规则3，decltype(0,i)推导类型为int&
  decltype(0, i);
  // i, 0表达式返回的是0，是纯右值，规则5，decltype(i,0)推导类型为int
  decltype(i, 0);

  // n[5]是左值，规则3，decltype(n[5])推导类型为int&
  decltype(n[5]);

  // *j是个左值（可以赋值），规则3，decltype(*j)推导类型为int&
  decltype(*j);

  // static_cast<int&&>(i)转换后是一个将亡值，decltype(static_cast<int&&>(i))推导类型为int&&
  decltype(static_cast<int&&>(i));

  // i++，返回的是原来的i，之后i再加1，返回的是纯右值，规则5，decltype(i++)推导类型为int
  decltype(i++);

  // ++i，先执行i加1，然后再返回i，返回的是左值，规则3，decltype(++i)推导类型为int&
  decltype(++i);

  // "hello world"是常量数组的左值，规则3，decltype("hello world")推导类型为
  // const char(&)[12]
  decltype("hello world");

  {
    int i = 100;
    int j;
    j = (1 + 2, i);
    std::cout << j << std::endl;  // 100

    j = (i, 1 + 2);
    std::cout << j << std::endl;  // 3
  }
}