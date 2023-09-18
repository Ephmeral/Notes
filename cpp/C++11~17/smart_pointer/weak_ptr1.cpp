// weak_ptr1.cpp
#include <iostream>
#include <memory>

int main() {
    auto sp = std::make_shared<int>(100);
    std::cout << "sp的引用计数为：" << sp.use_count() << std::endl;
    std::weak_ptr<int> wp(sp); // 创建一个weak_ptr
    std::cout << "sp的引用计数为：" << sp.use_count() << std::endl;
    std::cout << "wp的引用计数为：" << wp.use_count() << std::endl;
}