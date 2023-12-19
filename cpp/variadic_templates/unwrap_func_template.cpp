#include <iostream>

template<class T, class U>
T baz(T t, U u) {
  std::cout << t << ":" << u << std::endl;
  return t;
}

template<class ...Args>
void foo(Args ...args) {}

template<class ...Args>
class bar {
public:
  bar(Args ...args) {
    foo(baz(&args, args) ...);
  }
};

int main() {
  bar<int, double, unsigned int> b(1, 5.0, 8);
}