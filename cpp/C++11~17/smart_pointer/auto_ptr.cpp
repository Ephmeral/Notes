#include <iostream>
#include <memory>
#include <string>
using namespace std;

class People {
public:
    People(const char *name) : name_(name) {}

    std::string get_name() {
        return name_;
    }
private:
    const char* name_;
};

void test_1() {
    std::auto_ptr<string> p1 (new string ("Hello World"));   
    std::auto_ptr<string> p2;   
    p2 = p1; //auto_ptr不会报错.  
    // std::cout << *p1 << std::endl;
    std::cout << *p2 << std::endl;
}

void do_something(std::auto_ptr<People> people){
    // 该函数内不对people变量执行各种隐式/显示的所有权转移和释放
    // ...
}

void test_2() {
    std::auto_ptr<People> people(new People("Silas"));
    do_something(people);
    // ...

    std::cout << people->get_name() << std::endl;
}


int main() {
    test_1();
    test_2();
}