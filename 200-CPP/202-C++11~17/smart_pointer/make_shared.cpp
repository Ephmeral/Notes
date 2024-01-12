// make_shared.cpp
#include <iostream>
#include <vector>
#include <memory>

int main() {
    // 下面直接使用花括号初始化vector会编译失败，花括号不能完美转发
    // auto sp = std::make_shared<std::vector<int>>({10, 20}); 
    
    // 可以利用auto先创建一个std::initializer_list对象，再传递给make函数
    auto initList = {10, 20};
    auto spv = std::make_shared<std::vector<int>>(initList);
    for (auto it : *spv) {
        std::cout << it << std::endl;
    }
    return 0;
}