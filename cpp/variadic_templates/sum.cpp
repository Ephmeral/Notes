#include <iostream>
using namespace std;

template<typename T>
T sum(T arg) {
    return arg;
}

template<typename T, typename ...Args>
T sum(T arg1, Args ...args) {
    return arg1 + sum(args...);
}

// from C++17
template<typename ...Args>
auto sum2(Args ...args) {
    return (0 + ... + args );
}

template<typename ...Args>
void print(Args ...args) {
    (std::cout << ... << args) << std::endl;
}

int main() {
    std::cout << sum(1, 2, 5, 9) << std::endl;
    std::cout << sum2(1, 2, 5, 9) << std::endl;

    print(std::string("hello "), "c++ ", "world");

    std::cout << sum2() << std::endl;
}