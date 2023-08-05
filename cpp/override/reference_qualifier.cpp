// reference_qualifier.cpp 
#include <iostream>
using namespace std;

class Widget {
 public:
  // //只有*this为左值的时候才能被调用
  void doWork() & { std::cout << "this为左值，Widget::doWork &" << std::endl; }
  // 只有*this为右值的时候才能被调用
  void doWork() && { std::cout << "this为右值，Widget::doWork &&" << std::endl; }
};

// 工厂函数（返回右值）
Widget&& makeWidget() {
  Widget w;
  return std::move(w);
}

int main() {
  Widget w;  // 普通对象（左值）

  w.doWork();  // 调用被左值引用限定修饰的版本（即Widget::doWork &）
  makeWidget().doWork();  // 调用被右值引用限定修饰的版本（即Widget::doWork &&）
}
/*
$ g++ reference.cpp && ./a.out 
this为左值，Widget::doWork &
this为右值，Widget::doWork &&
*/