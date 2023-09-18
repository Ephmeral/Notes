#include <string>
#include <vector>
class City {
  std::string name;
  std::vector<std::string> street_name;

 public:
  City(const char *n) : name(n) {}
  City() {}  // 新添加的构造函数
};

int main() {
  City a("wuhan");
  City b;  // 编译失败，自定义构造函数抑制了默认构造函数
  b = a;
}