#include <iostream>
#include <type_traits>
using namespace std;

class NonTrivial {
  int i;

 public:
  NonTrivial(int n) : i(n), j(n) {}
  NonTrivial() {}
  int j;
};

class Trivial {
  int i;

 public:
  Trivial(int n) : i(n), j(n) {}
  Trivial() = default;
  int j;
};

int main() {
  Trivial a(5);
  Trivial b;
  b = a;
  std::cout << "std::is_trivial<Trivial>   : " << std::is_trivial<Trivial>()
            << std::endl;
  std::cout << "std::is_trivial<NonTrivial> : " << std::is_trivial<NonTrivial>()
            << std::endl;
}