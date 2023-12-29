#include <algorithm>
#include <iostream>
#include <list>
using namespace std;

template <typename T> std::list<T> quick_sort(std::list<T> v) {
  if (v.empty()) {
    return v;
  }
  std::list<T> res;
  res.splice(res.begin(), v, v.begin()); // 将 v 的首元素移到 res 中
  // 将 v 按条件划分为两部分，并返回第一个不满足条件元素的迭代器
  auto it = std::partition(v.begin(), v.end(),
                           [&](const T &x) { return x < res.front(); });
  std::list<T> low;
  low.splice(low.end(), v, v.begin(), it); // 转移左半部分到 low
  auto l(quick_sort(std::move(low)));      // 递归对左半部分快速排序
  auto r(quick_sort(std::move(v)));        // 递归对右半部分快速排序
  res.splice(res.end(), r);                // 右半部分移到结果后
  res.splice(res.begin(), l);              // 左半部分移到结果前
  return res;
}

int main() {
  std::list<int> ls = {2, 1, 5, 6, 9, 3, 4, 0};
  for (auto &x : quick_sort(ls)) {
    std::cout << x << " ";
  }
}