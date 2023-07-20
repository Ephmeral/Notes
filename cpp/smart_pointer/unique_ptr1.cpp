// unique_ptr1.cpp
#include <iostream>
#include <memory>

struct Foo {  // 要管理的对象
    Foo() { std::cout << "Foo ctor\n"; }
    Foo(const Foo&) { std::cout << "Foo copy ctor\n"; }
    Foo(Foo&&) { std::cout << "Foo move ctor\n"; }
    ~Foo() { std::cout << "~Foo dtor\n"; }
};

struct D {  // 删除器
    D(){};
    D(const D&) { std::cout << "D copy ctor\n"; }
    D(D&) { std::cout << "D non-const copy ctor\n"; }
    D(D&&) { std::cout << "D move ctor \n"; }
    void operator()(Foo* p) const {
        std::cout << "D is deleting a Foo\n";
        delete p;
    };
};

int main() {
    std::cout << "Example constructor(1)...\n";
    std::unique_ptr<Foo> up1;            // up1 为空
    std::unique_ptr<Foo> up1b(nullptr);  // up1b 为空

    std::cout << "Example constructor(2)...\n";
    {
        std::unique_ptr<Foo> up2(new Foo);  // up2 现在占有 Foo
    }                                       // Foo 被删除

    std::cout << "Example constructor(3)...\n";
    D d;
    {                                             // 删除器类型不是引用
        std::unique_ptr<Foo, D> up3(new Foo, d);  // 复制删除器
    }
    {                                               // 删除器类型是引用
        std::unique_ptr<Foo, D&> up3b(new Foo, d);  // up3b 保有到 d 的引用
    }

    std::cout << "Example constructor(4)...\n";
    {                                               // 删除器不是引用
        std::unique_ptr<Foo, D> up4(new Foo, D());  // 移动删除器
    }

    std::cout << "Example constructor(5)...\n";
    {
        std::unique_ptr<Foo> up5a(new Foo);
        std::unique_ptr<Foo> up5b(std::move(up5a));  // 所有权转移
    }

    std::cout << "Example constructor(6)...\n";
    {
        std::unique_ptr<Foo, D> up6a(new Foo, d);       // 复制 D
        std::unique_ptr<Foo, D> up6b(std::move(up6a));  // 移动 D

        std::unique_ptr<Foo, D&> up6c(new Foo, d);      // D 是引用
        std::unique_ptr<Foo, D> up6d(std::move(up6c));  // 复制 D
    }

    std::cout << "Example array constructor...\n";
    {
        std::unique_ptr<Foo[]> up(new Foo[3]);
    }  // 删除三个 Foo 对象
}