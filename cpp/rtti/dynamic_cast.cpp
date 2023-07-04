// data_types/dynamic_cast.cpp
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
    // 第一种情况，Base指针指向Derive对象，可以将Base指针转为Derive指针
    Derive derive(10);
    Base *bp = &derive;
    if (Derive *dp = dynamic_cast<Derive*>(bp)) { // 可以成功转型
        cout << "dynamic cast succeed~ ";
        dp->print();
    } else {
        cout << "dynamic cast failed!" << endl;
    }

    // 第二种情况，Base指针指向Base对象，不能将Base指针转为Derive指针
    Base base(15);
    bp = &base;
    if (Derive *dp = dynamic_cast<Derive*>(bp)) { // 转型失败
        cout << "dynamic cast succeed~ ";
        dp->print();
    } else {
        cout << "dynamic cast failed!" << endl;
    }

    // 第三种情况，Derive指针指向Derive对象，可以将Derive指针转为Base指针
    Derive *dp = new Derive(15);
    if (Base *bp = dynamic_cast<Base*>(dp)) { // 转型成功
        cout << "dynamic cast succeed~ ";
        bp->print();
    } else {
        cout << "dynamic cast failed!" << endl;
    }
    // 以下代码不能运行，不能将Derive指针指向Base对象
    // Derive *dp = new Base(20);
    return 0;
}
/*
$ g++ dynamic_cast.cpp && ./a.out
dynamic cast succeed~  i = 10
dynamic cast failed!
dynamic cast succeed~  i = 15
*/