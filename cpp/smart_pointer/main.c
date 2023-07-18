// dangling_pointer3.cpp
#include <stdio.h>

int main() {
    int *p;

    {
        int x = 10;
        p = &x;
    }
    // 离开作用域之后p是一个悬空指针
    // 但是下面代码实际还能正常运行
    printf("*p = %d\n", *p);
    return 0;
}