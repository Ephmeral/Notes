#include <cstring>
#include <iostream>
using namespace std;

int main() {
  char str[] = "Hello World!";

  cout << "sizeof(str) = " << sizeof(str) << endl; // 13
  cout << "strlen(str) = " << strlen(str) << endl; // 12
}