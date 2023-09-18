// zero_to_pointer.cpp
#include <iostream>
#include <cstdio>
#include <cstdlib>
using namespace std;

int main() {
    int* x = 0;
    int* nx = NULL;
    char* c = 0;
    char* nc = NULL;

    printf("x = %p\n", x);
    printf("nx = %p\n", nx);
    printf("c = %p\n", c);
    printf("nc = %p\n", nc);

    return 0;
}
/*
$ g++ zero_to_pointer.cpp && ./a.out 
x = (nil)
nx = (nil)
c = (nil)
nc = (nil)
*/