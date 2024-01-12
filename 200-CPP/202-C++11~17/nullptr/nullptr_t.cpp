// nullptr_t.cpp
#include <cassert>
#include <iostream>
using namespace std;

int main() {
  std::nullptr_t null1, null2;

  char* ch = null1;
  char* ch2 = null2;
  assert(ch == 0);
  assert(ch == nullptr);
  assert(ch == null2);
  assert(null1 == null2);
  assert(nullptr == null1);

  // std::nullptr_t长度和void*一样
  assert(sizeof(std::nullptr_t) == sizeof(void*));

  std::cout << "&null1 = " << &null1 << std::endl;  // null1和null2是左值，可以成功获取对象指针，
  std::cout << "&null2 = " << &null2 << std::endl;  // 并且指针指向的内存地址不同

  std::cout << &nullptr << std::endl;
}