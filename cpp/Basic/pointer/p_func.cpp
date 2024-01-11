#include <iostream>
using namespace std;

int func(int x) {
    return x + 10;
}

int main() {
    int (*p)(int);

    p = &func;

    cout << p(5) << endl;
}