## 实现一个 String 类

问题：写一个简单的 String 类，包含构造函数，拷贝构造函数，移动构造函数，拷贝赋值函数，析构函数，通过通过指针初始化，在这个基础上实现一个 append 函数

实现思路：这个问题主要是想考查几种构造函数的写法，然后 append 的函数的话，考虑采用 STL vector 的思想，提前预留一定的空间，当添加的字符串长度超过这个空间的时候进行扩容，下面是我的代码实现，仅供参考

有一些注意点：  
- 1）采用 new 分配一个数组的时候，需要配套采用 delete[]，析构函数当中需要释放内存；
- 2）拷贝构造函数传递的参数是 const 引用类型，而移动构造函数不是引用类型；
- 3）拷贝、移动赋值运算符需要进行特判，防止自己给自己赋值导致对象失效的问题；

```cpp
// mystring.cpp
#include <iostream>
#include <string.h>
using namespace std;

class String {
public:
  String() : str_(nullptr), size_(0), capacity_(10) {}
  String(const char *s) : capacity_(strlen(s) * 2), size_(strlen(s)) {
    str_ = new char[capacity_];
    strcpy(str_, s);
  }
  ~String() {
    if (str_) {
      delete[] str_;
    }
  }
  // 深拷贝
  String(const String &rhs) : size_(rhs.size_), capacity_(rhs.capacity_) {
    str_ = new char[rhs.size_];
    strcpy(str_, rhs.str_);
  }
  
  String& operator=(const String &rhs) {
    if (this == &rhs) {
        return *this;
    }
    str_ = new char[rhs.size_];
    strcpy(str_, rhs.str_);
    size_ = rhs.size_;
    capacity_ = rhs.capacity_;
    return *this;
  }

  String(String &&rhs) : str_(rhs.str_), size_(rhs.size_), capacity_(rhs.capacity_) {
    // 销毁rhs对象
    rhs.str_ = nullptr;
    rhs.capacity_ = 0;
    rhs.size_ = 0;
  }
  
  String& operator=(String &&rhs) {
    if (this == &rhs) {
        return *this;
    }
    str_ = rhs.str_;
    capacity_ = rhs.capacity_;
    size_ = rhs.size_;
    // 销毁rhs对象
    rhs.str_ = nullptr;
    rhs.capacity_ = 0;
    rhs.size_ = 0;
    return *this;
  }

  void append(const char *s) {
    int len = strlen(s);
    if (len + size_ < capacity_) {
      // 注意添加的字符串可能比扩容2倍后还要大，所以这里用循环判断一下
      while (len + size_ < capacity_) {
        capacity_ *= 2;
      }
      char *newstr = new char[capacity_ * 2];
      strcpy(newstr, str_);
      delete[] str_; // 删除旧的字符串
      str_ = newstr;
      size_ += len;
    } else {
      strcpy(str_ + size_, s);
      size_ += len;
    }
  }

  int size() const { return size_; }

  int capacity() const { return capacity_; }

  void print() { 
    if (str_) {
        cout << str_;
    }
    cout << endl;
  }

private:
  int capacity_ = 0;
  int size_ = 0;
  char *str_;
};

int main() {
  const char *s = "hello ";
  String str1(s);
  str1.print();
  str1.append("world! ");
  str1.print();
  str1.append("this is a massage");
  str1.print();

  std::cout << "===================" << std::endl;
  String tmp = "this is a string";
  String str2 = tmp; // 测试拷贝构造函数
  str2.print();

  String str3;
  str3 = str2; // 测试拷贝赋值运算符
  str3.print();

  String str4 = std::move(str2); // 测试移动构造函数
  str4.print();
  cout << "str2 = ";
  str2.print(); // str2应该为空

  cout << "test str5" << endl;
  String str5;
  str5 = std::move(str4); // 测试移动赋值运算符
  str5.print();
  
  return 0;
}
```
