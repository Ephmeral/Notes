#include <iostream>
using namespace std;

int main() {
  // 下面4个值都为27
  int x1 = 27;
  int x2(27);
  int x3 = {27};
  int x4{27};

  auto x5 = 27;    // int, 27
  auto x6(27);     // int, 27
  auto x7 = {27};  // std::initializer_list<int>，只有一个元素，值为27
  auto x8{27};     // int, 27

  {
    auto x1 = {1, 2};    // x1类型为 std::initializer_list<int>
    auto x2 = {1, 2.0};  // 编译失败，花括号中元素类型不同
    auto x3{1, 2};       // 编译失败，不是单个元素
    auto x4 = {3};       // x4类型为std::initializer_list<int>
    auto x5{3};          // x5类型为int
  }
}