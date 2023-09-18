#include <memory>
#include <string>
#include <vector>

class City {
  std::string name;
  std::vector<std::string> street_name;

 public:
  City(){};
  ~City() {}
  City(const City &) {}
};

int main() {
  City a, c;
  City b = a;
  c = b;
}