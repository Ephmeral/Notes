#include <iostream>
using namespace std;

class Base {
public:
	Base() :i(1) {
		
	}
	virtual void print() {
		cout << "this is base function of print" << endl;
	}
	int i;
};

class D : public Base {
public:
	D() {
		print();	
	}

	virtual void print() override {
		cout << "this is Derived function of print" << endl;
		cout << "i = " << i << endl;
	}

};

int main() {
	D d;
	Base *b = new D();
	return 0;
}
