#include <iostream>
using namespace std;

class Widget {
public:
  Widget() {}
  Widget(int _x) : x(_x) {}

private:
  int x{0};   // 没问题，x初始值为0
  int y = 0;  // 也可以
  // int z(0);   // 错误！
};

int main() {
  int x(0);   // 使用小括号初始化
  int y = 0;  // 使用"="初始化
  int z{0};   // 使用花括号初始化
  // int z = {0};  // 使用"="和花括号，和花括号认为是等价的

  {
    double x, y, z;
    int sum1{x + y + z};  // 错误！double的和可能不能表示为int
  }

  {
    Widget w1(10);  // 使用实参10调用Widget的一个构造函数
    Widget w2();  // 最令人头疼的解析！声明一个函数w2，返回Widget
    Widget w3{};  // 调用没有参数的构造函数构造对象
  }
}