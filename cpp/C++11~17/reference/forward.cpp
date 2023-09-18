// forward.cpp
#include <iostream>
using namespace std;

void RunCode(int &&m) { cout << "rvalue ref" << endl; }
void RunCode(int &m) { cout << "lvalue ref" << endl; }
void RunCode(const int &&m) { cout << "const rvalue ref" << endl; }
void RunCode(const int &m) { cout << "const lvalue ref" << endl; }

template <typename T>
void PerfectForward(T &&t) { RunCode(forward<T>(t)); }

int main() {
    int a;
    int b;
    const int c = 1;
    const int d = 0;
    
    PerfectForward(a);        // lvalue ref
    PerfectForward(move(b));  // rvalue ref
    PerfectForward(c);        // const lvalue ref
    PerfectForward(move(d));  // const rvalue ref
}
/*
$ g++ forward.cpp && ./a.out 
lvalue ref
rvalue ref
const lvalue ref
const rvalue ref
*/