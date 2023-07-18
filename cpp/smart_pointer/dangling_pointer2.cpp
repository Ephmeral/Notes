// dangling_pointer2.cpp
#include <iostream>

int* func() {
    int x = 10;
    // 返回x的地址，但是这个函数作用域结束后x对象的内存会被回收
    return &x; 
}

int main() {
    int *p = func(); // p是一个悬空指针

    std::cout << *p << std::endl; // 段错误
    return 0;
}