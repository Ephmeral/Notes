#include <iostream>
using namespace std;

void Func(char str[]) {
  printf("数组参数 sizeof = %ld\n", sizeof(str)); // 数组做参数退化为 char类型指针，即 8 个字节，总结第2点
}

struct AlignedStruct {
  char a;  // 本来1字节，padding 3 字节
  int b;   //  4 字节
  short c; // 本来 short 2字节，但是整体需要按照 4
           // 字节对齐(成员对齐边界最大的是int 4) ，
           //所以需要 padding 2，总共: 4 + 4 + 4
};

int main() {
  {
    // 前提: 64 位电脑上
    char str[] = "Hello World";
    char *p = str;
    double *dp;
    int n = 10;

    cout << sizeof(str) << endl; // 11个字符 + 末尾'\0'，总结第四点
    cout << sizeof(p) << endl;   // 64 位电脑，指针 8 个字节
    cout << sizeof(n) << endl;   // int 一般 4 个字节
  }

  {
    void *vp = malloc(100);
    cout << sizeof(vp) << endl; // vp 是一个 void 类型指针，还是 8 个字节
  }

  {
    char str[10] = "qerwqrwe";
    Func(str);
    cout << sizeof(AlignedStruct) << endl;
  }
  return 0;
}