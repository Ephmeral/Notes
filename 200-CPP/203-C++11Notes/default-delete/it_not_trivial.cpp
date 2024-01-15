#include <iostream>
#include <type_traits>
using namespace std;

class Trivial {
  int i;

 public:
  Trivial(int n) : i(n), j(n) {}
  Trivial() {}
  int j;
};

int main() {
  Trivial a(5);
  Trivial b;
  b = a;
  std::cout << "std::is_trivial<Trivial>   : "
            << std::is_trivial<Trivial>() << std::endl;
}
/*
$ g++ is_not_trivial.cpp && ./a.out 
std::is_trivial<Trivial>   : 0
*/