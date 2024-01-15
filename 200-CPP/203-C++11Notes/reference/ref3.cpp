// ref3.cpp
#include <iostream>
using namespace std;

int ReturnRvalue() {
    return 100; // 返回一个右值
}

int main() {
    // 下面代码正常运行，右值引用绑定到右值上，没有问题
    int &&rval = ReturnRvalue(); 
    
    int lval = 200; // lval是个左值
    // 下面代码会报错，不能将左值绑定到右值上
    int &&rval2 = lval;
    return 0;
}