// example.cpp
#include <iostream>
using namespace std;

struct Point {
  int x, y;
};

bool f(const Point& p) {
  if (p.x >= 0 && p.y >= 0) {
    return true;
  }
  return false;
}

int main() {
  const int i = 0;
  decltype(i) j = 0;  // j类型为const int

  decltype(f) fn;  // fn类型为bool(const Point&);

  Point p1(10, 20);
  decltype(p1) p2;  // p2类型为Point

  if (fn(p1)) {
    //...
  }

  decltype(fn(p1)) res;  // res类型为bool
}