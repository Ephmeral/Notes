// vfunc1.cpp
#include <iostream>
using namespace std;

class Base {
 public:
  virtual void some_func() {}
  virtual void foo(int x) {}
  virtual void bar() const {}
  void baz() {}
};

class Derived : public Base {
 public:
  virtual void sone_func() {}  // 字母写错了，some_func -> sone_func
  virtual void foo(int &x) {}  // 形参列表不同
  virtual void bar() {}        // 常量性不同
  virtual void baz() {}        // 基类不是虚函数，不是重写
};