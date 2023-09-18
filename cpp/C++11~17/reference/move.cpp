// move.cpp
#include <iostream>
#include <cstring>
using namespace std;

class HugeMem {
public:
    HugeMem(int size) : sz(size > 0 ? size : 1) {
        c = new int[sz];
    }
    
    ~HugeMem() { delete[] c; }

    HugeMem(const HugeMem& rhs) : sz(rhs.sz) {
        c = new int[sz];
        memcpy(c, rhs.c, sizeof(sz));
    }

    HugeMem(HugeMem&& hm) : sz(hm.sz), c(hm.c) {
        hm.c = nullptr;
    }
    int* c;
    int sz;
};

class Moveable {
public:
    Moveable() : i(new int(3)), h(1024) {}
    ~Moveable() { delete i; }
    // 去掉下面的std::move会调用h的拷贝构造函数
    Moveable(Moveable&& m) : i(m.i), h(std::move(m.h)) {  // 强制转为右值，以调用移动构造函数
        m.i = nullptr;
    }
    int* i;
    HugeMem h;
};

Moveable GetTemp() {
    Moveable tmp = Moveable();
    cout << hex << "Huge Mem from " << __func__
         << " @" << tmp.h.c << endl;  // Huge Mem from GetTemp @0x603030
    return tmp;
}

int main() {
    Moveable a(GetTemp());
    cout << hex << "Huge Mem from " << __func__
         << " @" << a.h.c << endl;  // Huge Mem from main @0x603030
}
/*
$ g++ move.cpp -fno-elide-constructors && ./a.out
Huge Mem from GetTemp @0x5648a9038ed0
Huge Mem from main @0x5648a9038ed0
*/