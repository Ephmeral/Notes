// data_types/typeid.cpp
#include <iostream>
using namespace std;

class Base {
public:
    Base(int _i) : i(_i) {}
    virtual void print() {
        cout << " i = " << i << endl;
    }
private:
    int i;
};

class Derive : public Base {
public:
    Derive(int i) : Base(i) {}
};

int main() {
    Derive *dp = new Derive(10);
    Base *bp = dp; // 两个指针都指向Derive对象

    if (typeid(*bp) == typeid(*dp)) {
        cout << "bp和dp指向同一个类型的对象" << endl;
    }

    if (typeid(*bp) == typeid(Derive)) {
        cout << "bp指向的是Derive对象" << endl;
    }

    // 下面代码会抛出std::bad_typeid异常
    /*
    Base *p = nullptr;
    if (typeid(*p) == typeid(Base)) {
        cout << "p指向的对象是Base类型" << endl;
    } else {
        cout << "p指向的对象不是Base类型" << endl;
    }
    */
    return 0;
}
/*
$ g++ typeid.cpp && ./a.out
bp和dp指向同一个类型的对象
bp指向的是Derive对象
*/