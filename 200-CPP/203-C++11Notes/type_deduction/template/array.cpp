#include <iostream>
using namespace std;

template <typename T>
void f(T param) {}  // 传值形参的模板

int main() {
  const char name[] = "J. P. Briggs";  // name的类型是const char[13]
  const char* ptrToName = name;        // 数组退化为指针

  f(name);  // T和param会推导成什么类型?
}