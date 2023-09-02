#include<stdio.h>
int a;
static int b=10;
int main(int argc,char* argv[],char* envp[]){
    int a=10;//stack
    char* p=(char*)malloc(sizeof(char));//heap
    printf("a=%d,p=0x%x\n",a,p);//动态链接库，*.so
    getchar();//设置阻塞
    getchar();
    return 0;
}