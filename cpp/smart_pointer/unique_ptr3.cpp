// unique_ptr3.cpp
#include <iostream>
#include <memory>

struct Vec3 {
    int x, y, z;
    Vec3(int x = 0, int y = 0, int z = 0) noexcept : x(x), y(y), z(z) {}
    friend std::ostream& operator<<(std::ostream& os, const Vec3& v) {
        return os << '{' << "x:" << v.x << " y:" << v.y << " z:" << v.z << '}';
    }
};

int main() {
    // 使用默认构造函数。
    std::unique_ptr<Vec3> v1 = std::make_unique<Vec3>();
    // 使用匹配这些参数的构造函数
    std::unique_ptr<Vec3> v2 = std::make_unique<Vec3>(0, 1, 2);
    // 创建指向 5 个元素数组的 unique_ptr
    std::unique_ptr<Vec3[]> v3 = std::make_unique<Vec3[]>(5);

    std::cout << "make_unique<Vec3>():      " << *v1 << '\n'
              << "make_unique<Vec3>(0,1,2): " << *v2 << '\n'
              << "make_unique<Vec3[]>(5):   " << '\n';
    for (int i = 0; i < 5; i++) {
        std::cout << "     " << v3[i] << '\n';
    }
}
/*
$ g++ unique_ptr3.cpp -o unique_ptr && ./unique_ptr 
make_unique<Vec3>():      {x:0 y:0 z:0}
make_unique<Vec3>(0,1,2): {x:0 y:1 z:2}
make_unique<Vec3[]>(5):   
     {x:0 y:0 z:0}
     {x:0 y:0 z:0}
     {x:0 y:0 z:0}
     {x:0 y:0 z:0}
     {x:0 y:0 z:0}
*/