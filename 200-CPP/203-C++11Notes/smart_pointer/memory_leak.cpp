// memory_leak.cpp
#include <iostream>
#include <cstdio>
#include <cstdlib>

int main() {
    int *p = (int*)malloc(sizeof(int*));
    *p = 10;

    int *q = new int(10);
    std::cout << *q << std::endl;
    return 0;    
}