// reference_qualifier2.cpp 
#include <iostream>
#include <vector>
using namespace std;

using DataType = std::vector<int>;

class Widget {
public:
  DataType& data() { 
    std::cout << "拷贝data" << std::endl;
    return values; 
  }

private:
  DataType values;
};

// 工厂函数（返回右值）
Widget&& makeWidget() {
  Widget w;
  return std::move(w);
}

int main() {
  Widget w;  // 普通对象（左值）
  auto v1 = w.data(); // 拷贝 w.values到 v1
  auto v2 = makeWidget().data(); // 拷贝 Widget 里面的值到 v2
}
/*
$ g++ reference_qualifier2.cpp && ./a.out 
拷贝data
拷贝data
*/