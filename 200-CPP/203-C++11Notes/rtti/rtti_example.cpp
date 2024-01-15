// data_types/rtti_example.cpp
#include <iostream>
using namespace std;

class Base {
    friend bool operator==(const Base&, const Base&);
public:
    Base(int _i, int _j) : i(_i), j(_j) {}
protected:
    virtual bool equal(const Base&) const;
    int i;
    int j;
};

class Derive : public Base {
public:
    Derive(int _i, int _j, int _k) : Base(_i, _j), k(_k) {}
protected:
    bool equal(const Base&) const;
private:
    int k;
};

bool operator==(const Base &lhs, const Base &rhs) {
    // 如果typeid不相等返回false；否则调用equal()
    return typeid(lhs) == typeid(rhs) && lhs.equal(rhs);
}

bool Derive::equal(const Base &rhs) const {
    cout << "Derive::equal()" << endl;
    // 我们清楚这两个类型是相等的，所以转换过程不会抛出异常
    auto r = dynamic_cast<const Derive&>(rhs);
    // 执行比较两个Derive对象的操作并返回
    return this->i == r.i && this->j == r.j && this->k == r.k;
}

bool Base::equal(const Base &rhs) const {
    cout << "Base::equal()" << endl;
    // 执行比较Base对象的操作
    return this->i == rhs.i && this->j == rhs.j;
}

int main() {
    // 第一种情况：两个基类指针进行判断
    Base *bp1 = new Base(10, 20);
    Base *bp2 = new Base(10, 20);
    if (*bp1 == *bp2) {
        cout << "bp1和bp2相等" << endl;
    }
    // 第二种情况：一个基类指针和一个派生类指针，都指向派生类对象
    Base *bp = new Derive(10, 20, 30);
    Derive *dp = new Derive(10, 20, 30);
    if (*bp == *dp) {
        cout << "bp和dp指向的对象相同" << endl;
    }
    // 第三种情况：两个指针分别指向基类对象和派生类对象，他两不能直接判断
    Base *bp3 = new Base(10, 20);
    Base *bp4 = new Derive(10, 20, 30);
    if (*bp3 == *bp4) {
        cout << "bp3和bp4指向的对象相同" << endl;
    } else {
        cout << "bp3和bp4指向的对象不相同" << endl;
    }
    return 0;
}