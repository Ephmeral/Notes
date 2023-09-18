// nullptr.cpp
#include <iostream>
using namespace std;

struct Point {
    int x, y, z;
};

int main() {
    // nullptr可以赋值给任意类型的指针，表示空指针
    int *pi = nullptr;
    char *pc = nullptr;
    long *pl = nullptr;
    Point *p = nullptr;

    // nullptr不允许赋值给非指针类型
    // int x = nullptr; 

    // nullptr不允许用在算术表达式中
    // int x = 0 + nullptr; // 编译错误
    
    int y = 1;

    // nullptr不允许与任何非指针类型比较（0除外）
    // if (y == nullptr) {
    //     std::cout << "y是一个空指针" << std::endl;
    // }
    if (0 == nullptr) {
        std::cout << "0是一个空指针" << std::endl;
    }
}