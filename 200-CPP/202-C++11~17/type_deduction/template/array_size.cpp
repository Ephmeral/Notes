#include <iostream>
#include <array>
using namespace std;

// 在编译期间返回一个数组大小的常量值（//数组形参没有名字，
// 因为我们只关心数组的大小）
template <typename T, std::size_t N>
constexpr std::size_t arraySize(T (&)[N]) noexcept {
  return N;
}

int main() {
  int keyVals[] = {1, 3, 7, 9, 11, 22, 35};  // keyVals有七个元素

  int mappedVals1[arraySize(keyVals)];              // mappedVals1也有七个
  std::array<int, arraySize(keyVals)> mappedVals2;  // mappedVals2的大小为7
  cout << mappedVals2.size() << endl;
}