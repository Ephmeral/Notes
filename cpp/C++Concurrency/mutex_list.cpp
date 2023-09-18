#include <list>
#include <mutex>
#include <algorithm>
#include <iostream>
using namespace std;

std::list<int> some_list;
std::mutex some_mutex;

void add_to_list(int new_value) {
    std::lock_guard<std::mutex> guard(some_mutex);
    some_list.push_back(new_value);
}

bool list_contains(int value_to_find) {
    std::lock_guard<std::mutex> guard(some_mutex);
    return std::find(some_list.begin(), some_list.end(), value_to_find) != some_list.end();
}

int main() {
    for (int i = 0; i < 10; ++i) {
        add_to_list(i);
    }
    int nums[] = {3, 5, 9, 12, 15};
    for (int i = 0; i < 5; ++i) {
        if (list_contains(nums[i])) {
            std::cout << "list contains "<< nums[i] << std::endl;
        } else {
            std::cout << "list not contains " << nums[i] << std::endl;
        }
    }
    return 0;
}