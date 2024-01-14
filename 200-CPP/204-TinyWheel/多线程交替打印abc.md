## 问题：使用多线程交替打印abc

主要考察多线程的使用，因为需要交替打印，这里需要对线程进行阻塞和等待，用一个全局的互斥锁，一个条件变量，以及一个变量loop来控制交替的轮次。代码如下：


```cpp
// thread_abc.cpp
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>

// 使用互斥锁确保线程安全
std::mutex mtx;
std::condition_variable cv;
int loop = 0; 

// 打印函数
void printChar(int curr, int count) {
    std::unique_lock<std::mutex> lock(mtx);

    int i;
    while (i < count) {
        // 不是当前线程执行的时候，该线程进行阻塞等待
        while (loop != curr) {
            cv.wait(lock);
        }
        loop = (loop + 1) % 3; // 转移给下一个线程
        char c = 'a' + curr;
        std::cout << c;
        ++i;
        cv.notify_all(); // 唤醒所有阻塞的线程
    }
}

int main() {
    // 创建三个线程，交替打印 'a', 'b', 'c' 共10次
    std::thread threadA(printChar, 0, 10);
    std::thread threadB(printChar, 1, 10);
    std::thread threadC(printChar, 2, 10);

    // 等待所有线程执行完毕
    threadA.join();
    threadB.join();
    threadC.join();

    std::cout << std::endl;  // 输出换行以使结果更清晰
    return 0;
}
```