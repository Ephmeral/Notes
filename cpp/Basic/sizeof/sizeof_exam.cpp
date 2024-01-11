#include <iostream>
using namespace std;

struct A {
    int a; 
    double b;
    char c;
};

struct B {}; // empty class

int main() {
    {
        cout << "sizeof(A) = " << sizeof(A) << endl; // 24, 根据最大的double对齐为8字节
    }

    {
        int arr[10];
        cout << "sizeof(arr) = " << sizeof(arr) << endl; // 40, 4 * 10
    }

    {
        cout << "sizeof(B) = " << sizeof(B) << endl; // 1, 空类大小为1
    }

}