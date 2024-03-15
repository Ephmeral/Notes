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

    int i = 0;
    while (i < count) {
        std::unique_lock<std::mutex> lock(mtx);
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
    // 创建三个线程，分别打印 'a', 'b', 'c'
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
