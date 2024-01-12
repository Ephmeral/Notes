// const_ref_qualifier.cpp
#include <iostream>
using namespace std;

class Widget {
public:
    void func1() const {
    }   
    void func2() const & {
    } 
    // void func3() & const { // 这里编译错误
    // }
};

int main() {
    Widget w;
}