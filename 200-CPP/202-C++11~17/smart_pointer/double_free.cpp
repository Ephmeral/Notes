// double_free.cpp
#include <iostream>

int main() {
    int *p = new int(10);
    delete p;
    // ...
    delete p;
}