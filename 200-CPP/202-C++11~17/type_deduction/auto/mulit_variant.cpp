// mulit_variant.cpp
#include <iostream>
using namespace std;

int main() {
  int n = 5;
  auto *p = &n, q = n;  // p为int*，q为int

  auto *pp = &n, qq = 10.0;  // 类型不匹配
}