// vfunc2.cpp
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
  virtual void sone_func() override {}  // 字母写错了，some_func -> sone_func
  virtual void foo(int &x) override {}  // 形参列表不同
  virtual void bar() override {}        // 常量性不同
  virtual void baz() override {}        // 基类不是虚函数，不是重写
};
/*
$ g++ vfunc2.cpp 
vfunc2.cpp:14:16: error: ‘virtual void Derived::sone_func()’ marked ‘override’, but does not override
   virtual void sone_func() override {}  // 字母写错了，some_func -> sone_func
                ^~~~~~~~~
vfunc2.cpp:15:16: error: ‘virtual void Derived::foo(int&)’ marked ‘override’, but does not override
   virtual void foo(int &x) override {}  // 形参列表不同
                ^~~
vfunc2.cpp:16:16: error: ‘virtual void Derived::bar()’ marked ‘override’, but does not override
   virtual void bar() override {}        // 常量性不同
                ^~~
vfunc2.cpp:17:16: error: ‘virtual void Derived::baz()’ marked ‘override’, but does not override
   virtual void baz() override {}        // 基类不是虚函数，不是重写
                ^~~
*/