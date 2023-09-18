// initializer_list.cpp
#include <iostream>
using namespace std;

class Widget {
 public:
  Widget(int i, bool b) { std::cout << "Widget(int, bool)" << std::endl; }
  Widget(int i, double d) { std::cout << "Widget(int, double)" << std::endl; }
  Widget(std::initializer_list<double> il) {
    std::cout << "Widget(std::initializer_list<double>)" << std::endl;
  }

 private:
  int m_i;
  bool flag;
};

int main() {
  Widget w1(10, true);  // 调用第一个构造函数
  Widget w2{10, true};  // 也调用第一个构造函数
  Widget w3(10, 5.0);   // 调用第二个构造函数
  Widget w4{10, 5.0};   // 也调用第二个构造函数
}

