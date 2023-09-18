#include <iostream>
using namespace std;

class Base {
public:
  virtual void foo(int x) {}

};

class Derived : public Base {
public:
  void foo(int x) final {};
};

class Derived2 : public Derived {
public:
  void foo(int x) {};
};