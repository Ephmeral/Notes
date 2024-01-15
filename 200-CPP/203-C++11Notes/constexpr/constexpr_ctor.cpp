// constexpr_ctor.cpp
#include <iostream>

struct X {
    int x1;
};

class Y {
public:
    constexpr Y() : val(0) {}
    constexpr Y(int _val) : val(_val) {}
    constexpr int get() const { return val; }
private:
    int val;
};

int main() {
    constexpr X x = {100};
    char buf[x.x1] = {0};

    constexpr Y y = {100};
    char buf2[y.get()] = {0};

    int i = 8;
    constexpr X x(i);     // 编译失败，不能使用constexpr声明
    X y(i);               // 编译成功
}