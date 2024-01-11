#include <iostream>

#pragma pack(push, 1) // 设置字节对齐为 1 字节，取消自动对齐
struct UnalignedStruct {
    char a;
    int b;
    short c;
};
#pragma pack(pop) // 恢复默认的字节对齐设置

struct AlignedStruct {
    char a;   // 本来1字节，padding 3 字节
    int b;    //  4 字节
    short c;  // 本来 short 2字节，但是整体需要按照 4 字节对齐(成员对齐边界最大的是int 4) 
              // 所以需要padding 2
   // 总共: 4 + 4 + 4
};

struct MyStruct {
 double a;    // 8 个字节
 char b;      // 本来占一个字节，但是接下来的 int 需要起始地址为4的倍数
              //所以这里也会加3字节的padding
 int c;       // 4 个字节
 // 总共:  8 + 4 + 4 = 16
};

struct MyStruct1 {
 char b;    // 本来1个字节 + 7个字节padding
 double a;  // 8 个字节
 int c;     // 本来 4 个字节，但是整体要按 8 字节对齐，所以 4个字节padding
  // 总共: 8 + 8 + 8 = 24
};


int main() {
    std::cout << "Size of unaligned struct: " << sizeof(UnalignedStruct) << std::endl; 
    // 输出：7
    std::cout << "Size of aligned struct: " << sizeof(AlignedStruct) << std::endl; 
    // 输出：12，取决于编译器和平台
    std::cout << "Size of aligned struct: " << sizeof(MyStruct) << std::endl; 
    // 输出：16，取决于编译器和平台
    std::cout << "Size of aligned struct: " << sizeof(MyStruct1) << std::endl;
     // 输出：24，取决于编译器和平台
    return 0;
}