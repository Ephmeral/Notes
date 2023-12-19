#include <iostream>
#include <vector>
using namespace std;

int main() {
    std::vector<int> vec;
    cout << "vec size = " << vec.size() << " vec cap = " << vec.capacity() << endl;
    for (int i = 0; i < 9; ++i) {
        vec.push_back(i);
    }
    cout << "vec size = " << vec.size() << " vec cap = " << vec.capacity() << endl;
    vec.shrink_to_fit();
    cout << "vec size = " << vec.size() << " vec cap = " << vec.capacity() << endl;
}