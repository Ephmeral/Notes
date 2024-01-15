// shared_ptr3.cpp
#include <cstdio>
#include <iostream>
#include <memory>

void report(std::shared_ptr<int> ptr) {
    if (ptr) {
        std::cout << "*ptr=" << *ptr << "\n";
    } else {
        std::cout << "ptr is not a valid pointer.\n";
    }
}

int main() {
    {
        std::shared_ptr<int> ptr;
        report(ptr);
    }

    {
        std::shared_ptr<int> ptr(new int(7));
        report(ptr);
    }
    return 0;
}
/*
$ g++ shared_ptr3.cpp -o shared_ptr && ./shared_ptr 
ptr is not a valid pointer.
*ptr=7
*/