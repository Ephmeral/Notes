// is_reference.cpp
#include <iostream>
#include <utility>

class A {};

int main() {
    std::cout << std::boolalpha;
    std::cout << std::is_rvalue_reference<A>::value << std::endl;      // false
    std::cout << std::is_rvalue_reference<A&>::value << std::endl;     // false
    std::cout << std::is_rvalue_reference<A&&>::value << std::endl;    // true
    std::cout << std::is_rvalue_reference<int>::value << std::endl;    // false
    std::cout << std::is_rvalue_reference<int&>::value << std::endl;   // false
    std::cout << std::is_rvalue_reference<int&&>::value << std::endl;  // true
    return 0;
}