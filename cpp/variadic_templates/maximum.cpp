#include <iostream>
using namespace std;

int maximum(int n) {
    return n;
}

template<typename ...Args>
int maximum(int n, Args ...args) {
    return std::max(n, maximum(args...));
}

int main() {
    std::cout << maximum(1, 2, 3, 6, 0, 10, 15) << std::endl;
}