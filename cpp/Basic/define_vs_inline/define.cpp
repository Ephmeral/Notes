#include <iostream>
using namespace std;

#define MAX(a, b) ((a) > (b) ? (a) : (b))

inline int max(int a, int b) { return a > b ? a : b; }

int main() {
  int x = 1;
  int y = MAX(x++, 0);

  int xx = 1;
  int yy = max(xx++, 0);

  cout << "x = " << x << " y = " << y << endl;     // x = 3 y = 2
  cout << "xx = " << xx << " yy = " << yy << endl; // xx = 2 yy = 1
  return 0;
}