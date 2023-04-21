#include <iostream>
#include <thread>
#include <vector>
#include <functional>
using namespace std;

std::thread::id master_thread;

void do_master_thread_work() {
    cout << "this is a main thread, thread id = " 
         << std::this_thread::get_id() << endl;
}

void do_common_work() {
    cout << "this is a common thread, thread id = " 
         << std::this_thread::get_id() << endl;
}

void some_core_part_of_algorithm() {
    if(std::this_thread::get_id() == master_thread) {
        do_master_thread_work();
    }else {
        do_common_work();
    }
}

int main() {
    // 下面代码创建9个线程，分别调用核心函数
    // 其中通过获得线程id以区分主线程和子线程
    
    master_thread = std::this_thread::get_id();
    // 父线程调用核心算法
    some_core_part_of_algorithm(); 

    std::vector<std::thread> threads(9);
    for (int i = 0; i < 9; ++i) {
        threads[i] = std::thread(some_core_part_of_algorithm);
    }
    // 等待子线程结束
    std::for_each(threads.begin(), threads.end(),
       std::mem_fn(&std::thread::join));

    return 0;
}