#include <iostream>
using namespace std;

int main() {
    int a = 1, b = 2, c = 3;
    int* p[3];
    p[0] = &a;
    p[1] = &b;
    p[2] = &c;
    // p = 1 2 3
    std::cout << "p = " << *p[0] << " " << *p[1] << " " << *p[2] << std::endl;

    int d[3] = {0, 0, 0};
    int (*q)[3];
    q = &d; // q现在指向数组d
    (*q)[0] = a;
    (*q)[1] = b;
    (*q)[2] = c;
    // q = 1 2 3
    std::cout << "q = " << (*q)[0] << " " << (*q)[1] << " " << (*q)[2] << std::endl;
    // d = 1 2 3 可以看到修改(*q)[0]的内容之后，数组d当中的内容也随之修改
    std::cout << "d = " << d[0] << " " << d[1] << " " << d[2] << std::endl;
}