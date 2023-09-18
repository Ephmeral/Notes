// wild_pointer.cpp
#include <iostream>

int main() {
	int *p; /* p是野指针 */
    // 下面直接访问*p对象会出错
    // std::cout << *p << std::endl;

	int x = 10;
	// p现在不是野指针了
    p = &x;
    std::cout << *p << std::endl; // 10
	return 0;
}