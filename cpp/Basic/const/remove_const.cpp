#include <iostream>
using namespace std;

int main() {
    const volatile int a = 40;
    const volatile int *cp = &a;
    int *p = const_cast<int*>(cp);
    *p = 35;
    cout << "a = " << a << " *p = " << *p << endl;
}