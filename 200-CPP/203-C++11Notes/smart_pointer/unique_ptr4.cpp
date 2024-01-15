#include <memory>
#include <vector>
#include <iostream>
using namespace std;

int main() {
    vector<unique_ptr<int>> vec;
    vec.push_back(unique_ptr<int>(new int(11)));
    vec.push_back(unique_ptr<int>(new int(13)));

    unique_ptr<int> outter_p(new int(17));
    // vec.push_back(outter_p); // error
    vec.push_back(std::move(outter_p)); // ok
    
    if (outter_p) 
        cout << "outter = " << *outter_p << endl;

    for (auto &p : vec)
        cout << *p << endl;
}