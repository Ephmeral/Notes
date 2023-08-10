// constexpr_val.cpp
#include <iostream>
using namespace std;

int main() {
    // 使用consexpr定义变量x
    constexpr int x = 43;
    char buf[x] = {0};

    // 使用const定义z，下面的代码可能会编译不通过
    int y = 43;
    const int z = y;
    char buf2[z] = {0};    
}