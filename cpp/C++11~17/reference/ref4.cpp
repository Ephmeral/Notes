// ref4.cpp
#include <iostream>
using namespace std;

int main() {
    int a = 20;
    const int ca = 30;
    const int &cval1 = 100; // 常量左值引用可以绑定到右值上
    const int &cval2 = a;   // 常量左值引用可以绑定到非常量左值上
    const int &cval3 = ca;  // 常量左值引用可以绑定到常量左值上
    
    cout << "cval1 = " << cval1 << " cval2 = " << cval2 << " cval3 = " << cval3 << endl;
    return 0;
}