#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int cfd1, cfd2;
    char str1[] = "Hi~ \n";
    char str2[] = "It's nice day~\n";

    cfd1 = dup(1); // 为标准输出流复制一个文件描述符
    cfd2 = dup2(cfd1, 7); // 将cfd1复制一份，并指定为7号文件描述符

    printf("fd1 = %d, fd2 = %d\n", cfd1, cfd2);
    write(cfd1, str1, sizeof(str1));
    write(cfd2, str2, sizeof(str2));

    close(cfd1);
    close(cfd2);
    write(1, str1, sizeof(str1));
    close(1);
    write(1, str2, sizeof(str2)); // 1上面已经关闭了，此时没有任何输出
    return 0;
}