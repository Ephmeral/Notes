#include <iostream>
using namespace std;

int main() {
    const int a = 42;
    int* mutable_ptr = const_cast<int*>(&a); // 删除const属性，使得可以修改a的值
    *mutable_ptr = 43; // 修改a的值

    cout << "a = " << a << endl;

    const volatile int age = 39;

    //1.直接赋值，导致编译错误
    //age = 40;

    //2.使用指针修改
    int * p = (int *)&age;
    *p = 40;

    cout << "age=" << age << endl;
    cout << "*p=" << *p << endl;
    cout << "&age=" << &age << endl;
    cout << "p=" << (void*)p << endl;
    cout << "&p=" << &p << endl;

}