#include <iostream>

struct type {
  void *operator new(std::size_t) = delete;
};

type global_var;
int main() {
  static type static_var;
  type auto_var;
  // type *var_ptr = new type;  // 编译失败，该类的new已被删除
}