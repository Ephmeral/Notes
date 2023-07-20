// weak_ptr2.cpp
#include <iostream>
#include <memory>

int main() {
    auto sp = std::make_shared<int>(100);
    std::weak_ptr<int> wp(sp); // 创建一个weak_ptr
    
    if (std::shared_ptr<int> np = wp.lock()) { // 通过lock判断共享的对象是否存在
        std::cout << "wp指向的对象的值为：" << *np << std::endl;
    }

    sp = nullptr; // sp置为空

    if (std::shared_ptr<int> np = wp.lock()) {
        std::cout << "此时这个条件不会成立" << std::endl;
    } else {
        std::cout << "wp指向的对象已经销毁" << std::endl;
    }

    if (wp.expired()) {
        std::cout << "wp已经悬空，又称已经expired（过期）" << std::endl;
    }
}
/*
$ g++ weak_ptr2.cpp -o weak_ptr && ./weak_ptr
wp指向的对象的值为：100
wp指向的对象已经销毁
wp已经悬空，又称已经expired（过期）
*/