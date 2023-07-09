#include <iostream>
using namespace std;

int main() {
    int ival = 1024;
    int &ref = ival;  // ref指向ival（是ival的另外一个名字）
    int &ref2;        // 引用类型必须初始化，这里会报错
    return 0;
}