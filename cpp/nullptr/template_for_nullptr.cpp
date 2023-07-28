// template_for_nullptr.cpp
#include <iostream>
#include <memory>
#include <mutex>
using namespace std;
using MuxGuard = std::lock_guard<std::mutex>;

struct Widget {};

// 只能被合适的已锁互斥量调用
int f1(std::shared_ptr<Widget> spw) {
  std::cout << "f1(std::shared_ptr<Widget> spw)" << std::endl;
}

double f2(std::unique_ptr<Widget> upw) {
  std::cout << "f2(std::unique_ptr<Widget> upw)" << std::endl;
}

bool f3(Widget* pw) { std::cout << "f3(Widget* pw)" << std::endl; }

template <typename FuncType, typename MuxType, typename PtrType>
decltype(auto) lockAndCall(FuncType func, MuxType& mutex, PtrType ptr) {
  MuxGuard g(mutex);
  return func(ptr);
}

int main() {
  // 用于f1，f2，f3函数的互斥量
  std::mutex f1m, f2m, f3m;
  //   auto result1 = lockAndCall(f1, f1m, 0);        // 错误！
  //   auto result2 = lockAndCall(f2, f2m, NULL);     // 错误！
  auto result3 = lockAndCall(f3, f3m, nullptr);  // 没问题
}