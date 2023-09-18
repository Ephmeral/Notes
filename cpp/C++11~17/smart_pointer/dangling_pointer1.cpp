// dangling_pointer1.cpp
#include <cstdlib>
#include <iostream>

int main() {
	int* p = (int *)malloc(sizeof(int));

	// free释放内存后，p是一个悬空指针
	free(ptr);
	
	// p置为NULL，不再是悬空指针
	p = NULL;
}