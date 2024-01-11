#include <iostream>
using namespace std;

class A {
public:
    A() {}
    A(int i_) : i(i_) {}
    void print() {
        std::cout << "i = " << i << std::endl;
    }
    ~A() {}
private:
    void* operator new(size_t t) {

    }
    // 重载了new就需要重载delete  
    void operator delete(void* ptr) {

    } 

    
    int i;
};

int main() {
    // A *a = new A(1);
    A *a2 = new A[5];
    // a->print();

    A a1;
}