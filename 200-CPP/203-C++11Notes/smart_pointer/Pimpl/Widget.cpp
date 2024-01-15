#include "Widget.h"
#include "Gadget.h"

#include <memory>
#include <string>
#include <vector>


struct Widget::Impl {  // 跟之前一样
    std::string name;
    std::vector<double> data;
    Gadget g1, g2, g3;
};

Widget::Widget()                       // 根据条款21，通过std::make_unique
    : pImpl(std::make_unique<Impl>())  // 来创建std::unique_ptr
{}

Widget::~Widget() {
}

Widget::Widget(Widget&& rhs) = default;

Widget& Widget::operator=(Widget&& rhs) = default;