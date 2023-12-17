#include <iostream>
using namespace std;

class Widget {
public:
    virtual void flip() {
        std::cout << "Widget flip!" << std::endl;
    }
};

class Bell : public Widget {
public:
    void flip() {
        std::cout << "Bell flip!" << std::endl;
    }
};

class Whistle : public Widget {
public:
    void flip() {
        std::cout << "Whistle flip!" << std::endl;
    }
};

void flip(Widget& widget) {
    widget.flip();
}

int main() {
    Bell b;
    Whistle w;

    flip(b);
    flip(w);
}