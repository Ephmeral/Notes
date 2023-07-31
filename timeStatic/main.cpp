#include <iostream>

int add(int x, int y) {
    return x + y;
}

int main() {
    // 如果有个int add（int x，int y）类型的函数，使用decltype（add（0，0））的方式进行推导之后，得到的最终类型是int（void）类型吗
    decltype(add(0, 0)) fn;
    std::cout << fn << std::endl;
    std::cout << typeid(fn).name() << std::endl;
    return 0;
}