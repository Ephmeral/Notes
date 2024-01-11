#include <iostream>
using namespace std;


union A {
    int x;
    char c;
};

int main() {
    int x = 0x1;
    char *p = reinterpret_cast<char*>(&x);
    if (*p == 1) {
        cout << "小端存储" << endl;
    } else {
        cout << "大端存储" << endl;
    }

    A a;
    a.x = 0x12345678;
    if (a.c == 0x12) {
        cout << "大端存储" << endl;
    } else if (a.c == 0x78) {
        cout << "小端存储" << endl;
    }
}