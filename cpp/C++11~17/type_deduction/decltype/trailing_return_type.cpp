#include <iostream>
#include <vector>
using namespace std;

template <class T1, class T2>
auto sum(T1 t1, T2 t2) -> decltype(t1 + t2) {
  return t1 + t2;
}

template<typename Container, typename Index>
auto getItem(Container &c, Index id) -> decltype(c[id]) 
{
    return c[id];
}

int main() {
    auto x = sum(4, 2.0); // 返回值推导为double

    std::vector<int> nums{1, 2, 3, 4, 5};

    getItem(nums, 0) = 10;
    std::cout << nums[0] << std::endl;
}