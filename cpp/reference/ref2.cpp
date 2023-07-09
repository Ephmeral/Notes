// ref2.cpp
#include <iostream>
using namespace std;

int main() {
    int ival = 1024;
    int &ref = ival;  
    
    ref = 1000;
    cout << "ival = " << ival << endl;

    // int &&rref = ref; // 这里代码会报错，&&不是引用的引用，而是另外一种引用类型
    return 0;
}