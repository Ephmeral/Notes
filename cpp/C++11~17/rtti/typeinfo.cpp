// data_types/typeinfo.cpp
#include <iostream>
using namespace std;

class Base {
};

class Derive : public Base {
};

int main() {
    int arr[10];
    int *arrp = new int[10];
    Derive d;
    Base *p = &d;

    cout << typeid(42).name() << ", "
         << typeid(42.0).name() << ", "
         << typeid('a').name() << ", "
         << typeid("string").name() << ", "
         << typeid(std::string).name() << ", "
         << typeid(arr).name() << ", "
         << typeid(arrp).name() << ", "
         << typeid(d).name() << ", "
         << typeid(p).name() << ", "
         << typeid(*p).name() << endl;
    return 0;
}