class Base {
  void foo(long &);

 public:
  void foo(int) {}
};

class Derived : public Base {
 public:
  using Base::foo;
  void foo(const char *) {}
};

int main() {
  {
    Base b;
    long l = 5;
    b.foo(8);
    b.foo(l);  // 编译错误
  }
  {
    Derived d;
    d.foo("hello");
    d.foo(5);
  }
}