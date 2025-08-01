## 前言

互联网技术面一般会涉及手撕算法，除了常见的力扣 Hot100、剑指 Offer 外，可能还会有一些常见的设计题，主要考察基本代码能力，本文整理了一些 C++ 语言常见的题目，大部分版本为简单的实现，面试的时候建议和面试官沟通确定是否需要进阶实现。个人手动整理，如有不足，欢迎批评指正。

## 单例模式

常见问题：
- 绝对线程安全的单例（Meyers Singeleton）
- 设计一个线程安全的单例模式
- 懒汉单例模式的线程安全问题  
- 为什么要用锁加双重判断，只判断一次不可以吗？

### 什么是单例模式

**在一个项目中，全局范围内，某个类的实例有且仅有一个，通过这个唯一实例向其他模块提供数据的全局访问**，这种模式就叫单例模式。单例模式的典型应用就是任务队列。

### 如何实现一个单例模式

- 使用单例模式，首先要保证这个类的实例有且仅有一个，这个时候构造函数都需要设置为私有或删除的，因为不能通过构造函数来创建新的对象。
- 具体实现的时候，有两种实现方式：
	- **饿汉式**：饿汉模式就是在**类加载的时候立刻进行实例化**，这样就得到了一个唯一的可用对象。
	- **懒汉式**：是在类加载的时候不去创建这个唯一的实例，而是**在需要使用的时候再进行实例化**。

### 饿汉式单例模式实现代码

- 类的静态成员变量放在外面进行初始化，然后通过 getInstance() 函数返回实例

```cpp
// 饿汉模式
class TaskQueue {
public:
    // = delete 代表函数禁用, 也可以将其访问权限设置为私有
    TaskQueue(const TaskQueue& obj) = delete;
    TaskQueue& operator=(const TaskQueue& obj) = delete;
    static TaskQueue* getInstance() {
        return m_taskQ;
    }
private:
    TaskQueue() = default;
    static TaskQueue* m_taskQ;
};
// 静态成员初始化放到类外部处理
TaskQueue* TaskQueue::m_taskQ = new TaskQueue;
int main() {
    TaskQueue* obj = TaskQueue::getInstance();
}
```

### 懒汉式单例模式实现代码（基础版）

- 基本思路是在调用 getInstance() 函数的时候，判断类是否实例化了，没有的话，进行实例化；
- 但是下面的代码会有线程安全问题：

```cpp
// 懒汉模式
class TaskQueue {
public:
    // = delete 代表函数禁用, 也可以将其访问权限设置为私有
    TaskQueue(const TaskQueue& obj) = delete;
    TaskQueue& operator=(const TaskQueue& obj) = delete;
    static TaskQueue* getInstance() {
        if(m_taskQ == nullptr) {
            m_taskQ = new TaskQueue;
        }
        return m_taskQ;
    }
private:
    TaskQueue() = default;
    static TaskQueue* m_taskQ;
};
TaskQueue* TaskQueue::m_taskQ = nullptr;
```

### 双重检查锁单例模式

为什么是双重检查？如果是只检查一次的话，先上锁然后在判断是否实例化，这样的话后续其他线程在调用 getInstance() 函数的时候，都会阻塞在这行，效率比较低下。

```cpp
class TaskQueue {
public:
    // = delete 代表函数禁用, 也可以将其访问权限设置为私有
    TaskQueue(const TaskQueue& obj) = delete;
    TaskQueue& operator=(const TaskQueue& obj) = delete;
    static TaskQueue* getInstance() {
        if (m_taskQ == nullptr) {
            m_mutex.lock();
            if (m_taskQ == nullptr) {
                m_taskQ = new TaskQueue;
            }
            m_mutex.unlock();
        }
        return m_taskQ;
    }
private:
    TaskQueue() = default;
    static TaskQueue* m_taskQ;
    static mutex m_mutex;
};
TaskQueue* TaskQueue::m_taskQ = nullptr;
mutex TaskQueue::m_mutex;
```

### C++ 双重检查锁单例模式一定是安全的吗？

- 不是，因为指令可能会重排，假设第一步是分配 TaskQueue 内存，第二步是在内存中构造 TaskQueue 对象，第三步是 m_taskQ 指针指向分配的内存；
- 重排后可能导致 m_taskQ 先指向分配的内存，之后在构造对象，而到了第二步，如果这个时候线程时间片耗尽，转而执行另外一个线程，另外一个线程在检查的时候，发现指针不为空，但是实际上这个时候对象还没初始化完成，这就出现问题了；
- 解决方法：
	- 使用 `call_once` 和 `once_flag` 来确保初始化只发生一次；
	- 另外一种是使用局部静态变量，因为 C++11 规定：如果指令逻辑进入一个未被初始化的声明变量，所有并发执行应当等待该变量完成初始化。所以局部静态变量也是线程安全的。

```cpp
class Singleton {
private:
    static Singleton instance;
    static std::once_flag onceFlag;
    explicit Singleton() {
        // 构造函数私有化，防止直接实例化
    }
public:
    static Singleton& getInstance() {
        std::call_once(onceFlag, []() {
            instance = Singleton();
        });
        return instance;
    }
};

// 静态成员初始化
Singleton Singleton::instance;
std::once_flag Singleton::onceFlag;
// 示例使用
int main() {
    Singleton& singleton = Singleton::getInstance();
    // 使用singleton...
    return 0;
}
```

以下是使用局部静态变量：

```cpp
class TaskQueue {
public:
    // = delete 代表函数禁用, 也可以将其访问权限设置为私有
    TaskQueue(const TaskQueue& obj) = delete;
    TaskQueue& operator=(const TaskQueue& obj) = delete;
    static TaskQueue* getInstance() {
        static TaskQueue taskQ;
        return &taskQ;
    }
    void print() {
        cout << "hello, world!!!" << endl;
    }
private:
    TaskQueue() = default;
};

int main() {
    TaskQueue* queue = TaskQueue::getInstance();
    queue->print();
    return 0;
}
```

## 多线程交替打印 abc

问题：使用 3 个线程交替打印输出 abc 字符

主要考察多线程的使用，因为需要交替打印，这里需要对线程进行阻塞和等待，用一个全局的互斥锁，一个条件变量，以及一个变量 loop 来控制交替的轮次。代码如下：

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

## 生产者消费者模型

一个简单的生产者消费者模型，缓冲区大小为 10，生产者不断的往缓冲区中加入数据，直到容量满，消费者不断的从缓冲区中读取数据，一旦缓冲区为空就会阻塞。

```cpp
#include <algorithm>
#include <iostream>
#include <thread>
#include <condition_variable>
#include <mutex>
#include <queue>
using namespace std;

int N = 100;

const int MAX_SIZE = 10;

std::condition_variable cv;
std::mutex mtx;
std::queue<int> que;

void producer() {
    for (int i = 0; i < N; ++i) {
        std::unique_lock<mutex> lk(mtx);
        cv.wait(lk, []() {
            return que.size() <= MAX_SIZE;
        });
        std::cout << std::this_thread::get_id() << " 生成数据 " << i << endl;
        que.push(i);
        cv.notify_one();
    }
}

void consumer() {
    for (int i = 0; i < N; ++i) {
        std::unique_lock<mutex> lk(mtx);
        cv.wait(lk, []() {
            return !que.empty();
        });
        std::cout << std::this_thread::get_id() << " 消耗数据 " << que.front() << endl;
        que.pop();
        cv.notify_one();
    }
}

int main() {
    std::thread t1(producer);
    std::thread t2(consumer);

    t1.join();
    t2.join();
}
```

## 线程池

### 基础版线程池

一个比较简单的线程池，提交的任务函数类型为 `std::function<void()>`，实现如下：

```cpp
#include <iostream>
#include <vector>
#include <queue>
#include <functional>
#include <thread>
#include <condition_variable>
#include <mutex>
using namespace std;

class ThreadPool {
    using TaskType = std::function<void()>;
public:
    ThreadPool(int n) : nthread(n) {
        works.reserve(n);
        for (int i = 0; i < n; ++i) {
            works.push_back(thread([this]() {
                while (true) {
                    std::unique_lock<mutex> lk(mtx);
                    cv.wait(lk, [this]() {
                        return !tasks.empty() || stop;
                    });
                    if (tasks.empty() && stop) {
                        return;
                    }
                    TaskType task = std::move(tasks.front());
                    tasks.pop();
                    task();
                }
            }));
        }
    }

    ~ThreadPool() {
        {
            std::unique_lock<mutex> lk(mtx);
            stop = true;
        }
        cv.notify_all();
        for (int i = 0; i < nthread; ++i) {
            works[i].join();
        }
    }

    void setStop(bool flag = true) {
        {
            std::unique_lock<mutex> lk(mtx);
            stop = true;
        }
        cv.notify_all();
    }

    void addTask(TaskType task) {
        {
            std::unique_lock<mutex> lk(mtx);
            tasks.push(task);
        }
        cv.notify_one();
    }

private:
    int nthread;
    bool stop = false; // 默认是没退出
    std::mutex mtx;
    std::condition_variable cv;
    std::vector<thread> works;
    std::queue<TaskType> tasks;
};

int main() {
    ThreadPool pool(10);
    for (int i = 0; i < 100; ++i) {
        pool.addTask([x = i]() { // 注意这里变量的捕获需要用值捕获
            cout << std::this_thread::get_id() << " 正在处理任务 " << x << endl;
        });
    }
    return 0;
}
```

### 进阶版线程池

更加复杂的线程池：提交的任务函数类型是任意的，最终通过 `std::bind` 等封装为 `std::function<void()>` 类型

```cpp
#include <vector>
#include <thread>
#include <iostream>
#include <condition_variable>
#include <mutex>
#include <functional>
#include <future>
#include <queue>
#include <memory>
using namespace std;

class ThreadPool{
    using TaskType = std::function<void()>;
private:
    std::vector<thread> pools;
    std::queue<TaskType> tasks;
    std::mutex mtx;
    std::condition_variable cv;
    bool flag = false;

public:
    ThreadPool(int poolsize) {
        for (int i = 0; i < poolsize; ++i) {
            pools.push_back(thread([this]() {
                while (true) {
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lk(mtx);
                        cv.wait(lk, [this]() {
                            return !tasks.empty() || flag;
                        });

                        if (tasks.empty() && flag) {
                            return;
                        }
                        task = std::move(tasks.front());
                        tasks.pop();
                    }
                    
                    task();
                }
            }));
        }
    }

    ~ThreadPool() {
        stop();
    }

    template<typename F, class...Args>
    auto submit(F &&func, Args&& ...args) -> std::future<decltype(func(args...))> {
        using ReturnType = decltype(func(args...));

        auto task = std::make_shared<std::packaged_task<ReturnType()>>(
            std::bind(std::forward<F>(func), std::forward<Args>(args)...)
        );

        std::future<ReturnType> result = task->get_future();

        {
            std::unique_lock<std::mutex> lk(mtx);
            tasks.emplace([task]() {
                (*task)();
            });
        }

        cv.notify_one();
        return result;
    }

    void stop() {
        {
            std::unique_lock<std::mutex> lk(mtx);
            flag = true;
        }
        cv.notify_all();
        for (int i = 0; i < pools.size(); ++i) {
            pools[i].join();
        }
    }
};

void exampleFunc(int n, int m) {
    cout << "thread = " << std::this_thread::get_id() << " n = " << n << " m = " << m << endl;
}

int main() {
    ThreadPool threadpool(3);
    std::vector<std::future<void>> results;

    for (int i = 0; i < 10; i++) {
        results.emplace_back(threadpool.submit(exampleFunc, i, i * 10));
    }

    for (auto &result : results) {
        result.get();
    }
}
```

## 大小端判断

### 大端序、小端序是什么？

- **大端序**：多字节数据的最高有效字节存放在最低的内存地址，而最低有效字节存放在最高的内存地址。例如：`0x1234`，其中 `12` 在第一个字节，`34` 在第二个字节。
- **小端序**：多字节数据的最低有效字节存放在最低的内存地址，而最高有效字节存放在最高的内存地址。例如：`0x1234`，其中 `34` 在第一个字节，`12` 在第二个字节
- 大端序更加符合我们人类的读取习惯，如果地址从低往高来看的话，`0x1234` 就是按照我们从左往右的顺序读的，小端序更加符合计算机的习惯。

### 网络当中传输用的是什么字节序？

- 大端序

### 为什么字节序重要？

字节序的概念对于跨平台的数据交换非常重要，特别是在网络通信和文件传输中。网络协议通常规定使用**大端序（例如在以太网和 IP 协议中）**，而**不同的计算机架构可能使用不同的字节序**。因此，当数据在不同系统之间传输时，可能需要进行字节序转换（也称为字节序标准化或网络字节序转换）以确保数据的正确解释。

### 如何判断系统是大端序还是小端序

通过强制类型转换将整型低位的内容转为字符串，然后进行判断：

```cpp
#include <stdio.h>
int main() {
    unsigned int x = 0x12345678;
    char *c = (char*)&x;
    if (*c == 0x78) {
        printf("系统是小端序\n");
    } else {
        printf("系统是大端序\n");
    }
    return 0;
}
```

或使用 C 语言联合体的性质：

```cpp
#include <stdio.h>
union {
    unsigned int u;
    unsigned char c[4];
} testend;
int main() {
    testend.u = 0x12345678;
    if (testend.c[0] == 0x12) {
        printf("大端序（Big-Endian）\n");
    } else if (testend.c[0] == 0x78) {
        printf("小端序（Little-Endian）\n");
    } else {
        printf("无法确定字节序\n");
    }
    return 0;
}
```

## 智能指针的实现

简易版的智能指针实现如下：

### unique_ptr 的实现

unique_ptr 比较简单，主要是几个方法要记住是干什么的，代码如下：

```cpp
#include <iostream>
using namespace std;

template <typename T>
class UniquePtr {
private:
    T* ptr;  // 内部指针

public:
    // 构造函数：接受一个指向 T 类型的指针
    explicit UniquePtr(T* p = nullptr) : ptr(p) {}

    // 禁止拷贝构造和赋值操作
    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    // 移动构造函数
    UniquePtr(UniquePtr&& other) noexcept : ptr(other.ptr) {
        other.ptr = nullptr;  // 将源指针置为 nullptr
    }

    // 移动赋值操作符
    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this != &other) {  // 防止自我赋值
            delete ptr;        // 释放当前持有的资源
            ptr = other.ptr;   // 转移资源的所有权
            other.ptr = nullptr;  // 将源指针置为 nullptr
        }
        return *this;
    }

    // 重载解引用操作符
    T& operator*() const {
        return *ptr;
    }

    // 重载箭头操作符
    T* operator->() const {
        return ptr;
    }

    // 获取原始指针
    T* get() const {
        return ptr;
    }

    // 释放所拥有的指针并返回它
    T* release() {
        T* temp = ptr;
        ptr = nullptr;
        return temp;
    }

    // 重置指针，释放当前指针并拥有新的指针
    void reset(T* p = nullptr) {
	    if (ptr) {
		    delete ptr;
		}     
        ptr = p;
    }

    // 析构函数，自动释放内存
    ~UniquePtr() {
	    if (ptr) {
		    delete ptr;
		}        
    }
};

// 测试 unique_ptr 实现
int main() {
    UniquePtr<int> p1(new int(42));  // 创建一个 unique_ptr

    std::cout << *p1 << std::endl;  // 输出 42

    UniquePtr<int> p2 = std::move(p1);  // 移动所有权
    if (p1.get() == nullptr) {
        std::cout << "p1 is now nullptr" << std::endl;
    }

    std::cout << *p2 << std::endl;  // 输出 42

    p2.reset(new int(100));  // 重置指针
    std::cout << *p2 << std::endl;  // 输出 100

    return 0;
}
```

### shared_ptr 的实现

核心是引用计数原理，注意细节部分：
- 拷贝赋值和移动赋值的时候需要将原来的指针进行释放；
- 移动赋值运算符参数不能加 const，返回值和拷贝赋值运算符一样是引用类型；
- 原子变量获得值的方法是 load()

```cpp
#include <iostream>
#include <memory>
#include <atomic>
using namespace std;

template<typename T>
class SharedPtr {
private:
    T* ptr;     // 原始指针
    std::atomic<int>* ref_count; // 引用计数

    void release() {
        if (ref_count) {
            (*ref_count)--;
            if (*ref_count == 0) {
                delete ptr;
                delete ref_count;
            }
        }
    }

public: 
    SharedPtr() : ptr(nullptr), ref_count(nullptr) {}
    explicit SharedPtr(T *p) : ptr(p), ref_count(new std::atomic<int>(1)) {}

    ~SharedPtr() {
        release();
    }
    
    SharedPtr(const SharedPtr& rhs) : ptr(rhs.ptr), ref_count(rhs.ref_count) {
        if (ref_count) {
            (*ref_count)++;
        }
    }
    
    SharedPtr(SharedPtr&& rhs) : ptr(rhs.ptr), ref_count(rhs.ref_count) {
        rhs.ptr = nullptr;
        rhs.ref_count = nullptr;
    }

    SharedPtr& operator=(const SharedPtr& rhs) {
        if (this != &rhs) {
            release(); // 释放当前指针
            ptr = rhs.ptr;
            ref_count = rhs.ref_count;
            if (ref_count) {
                (*ref_count)++;
            }
        }
        return *this;
    }
    
    SharedPtr& operator=(SharedPtr&& rhs) {
        if (this != &rhs) {
            release();
            ptr = rhs.ptr;
            ref_count = rhs.ref_count;
            rhs.ptr = nullptr;
            rhs.ref_count = nullptr;
        }
        return *this;
    }

    T& operator*() const {
        return *ptr;
    }

    T* operator->() const {
        return ptr;
    }

    int use_count() const {
        return ref_count ? ref_count->load() : 0;
    }
};


template<typename T, class...Args>
SharedPtr<T> makeShared(Args&&...args) {
    return SharedPtr<T>(new T(std::forward<Args>(args)...));
}

class A {
public:
    A(int a, int b) : ma(a), mb(b) {}

    void print() {
        cout << "a = " << ma << " b = " << mb << endl;
    }
private:
    int ma, mb;
};

// 测试 shared_ptr 实现
int main() {
    SharedPtr<int> p1(new int(42));  // 创建一个 shared_ptr
    std::cout << "p1 use_count: " << p1.use_count() << std::endl;  // 输出引用计数

    {
        SharedPtr<int> p2 = p1;  // 拷贝构造
        std::cout << "p1 use_count: " << p1.use_count() << std::endl;  // 输出引用计数
        std::cout << "p2 use_count: " << p2.use_count() << std::endl;  // 输出引用计数
    }  // p2 离开作用域，引用计数减 1

    std::cout << "p1 use_count: " << p1.use_count() << std::endl;  // 输出引用计数

    SharedPtr<int> p3 = std::move(p1);  // 移动构造
    std::cout << "p1 use_count: " << p1.use_count() << std::endl;  // 输出引用计数
    std::cout << "p3 use_count: " << p3.use_count() << std::endl;  // 输出引用计数
  

    SharedPtr<A> pa = makeShared<A>(1, 2);
    pa->print();
    
    return 0;
}
```

## 内存池的实现

这里只实现了一个简单版本的内存池，基本思路，开辟一块大的空间，然后通过嵌入式指针的手法将每个块的前 8 个字节作为指针，存放下一块的地址，通过空闲链表串联起来。

```cpp
class MemoryPool {
public:
  MemoryPool(size_t size, size_t count) : blocksize(size), blockcount(count) {
    memory = (char*)malloc(blocksize * blockcount);
    freelist = memory; // 初始化，freelist指向内存池第一块内容

    char *curr_block = memory;
    for (size_t i = 0; i < count - 1; ++i) {
      // 将每个块的前8个字节作为指针，存放下一个块的地址
      *(void**)curr_block = (void*)(curr_block + blocksize);
      curr_block += blocksize;
    }
    *(void**)curr_block = nullptr;
  }

  ~MemoryPool() {
    free(memory);
  }

  void* allocateBlock() {
    if (freelist == nullptr) {
      return nullptr;
    }
    void *block = freelist;
    freelist = *(void**)block; // freelist指向下一个指针
    return block;
  }

  void deallocate(void* ptr) {
    // 检查内存是否在内存池的范围之内
    if (!(ptr >= memory && ptr < memory + blocksize * blockcount)) {
      return;
    }
    // 将块插入空闲链表的头部
    *(void**)ptr = freelist;
    freelist = ptr;
  }

private:
  char *memory = nullptr;
  void *freelist = nullptr;
  size_t blocksize;
  size_t blockcount;
};

int main() {
  {
    MemoryPool pool(sizeof(int*), 1000);
    int *p = (int*)pool.allocateBlock();

    *p = 10;

    int *p2 = (int*)pool.allocateBlock();
    *p2 = *p;

    std::cout << "&p = " << p << " *p = " << *p << std::endl;
    std::cout << "&p2 = " << p2 << " *p2 = " << *p2 << std::endl;
  }
}
```

## 线程安全的 HashMap

实现一个线程安全的 HashMap，这里采用的是开链法，整体不难实现，hash 函数那个可能要注意一下写法。这里没有进一步实现扩容操作了，如果需要实现整体会更加复杂。

```cpp
#include <vector>
#include <iostream>
#include <list>
#include <mutex>
#include <thread>
using namespace std;

template<typename KeyType, typename ValueType>
class HashMap {
private:
    struct Bucket {
        std::list<std::pair<KeyType, ValueType>> ls;
        std::mutex mtx;
    };
    std::vector<Bucket> table;

    size_t hashFunction(const KeyType &key) {
        return std::hash<KeyType>()(key) % table.size();
    }

public:
    HashMap(size_t size = 1001) : table(size) {}

    void insert(const KeyType& key, const ValueType& value) {
        size_t index = hashFunction(key);
        auto &bucket = table[index];
        std::unique_lock<std::mutex> lk(bucket.mtx);
        for (auto &kv : bucket.ls) {
            if (kv.first == key) {
                kv.second = value;
                return; // 键存在
            }
        }
        bucket.ls.emplace_back(key, value); // 键不存在
    }

    bool get(const KeyType& key, ValueType& value) {
        size_t index = hashFunction(key);
        auto &bucket = table[index];
        std::unique_lock<std::mutex> lk(bucket.mtx);
        for (auto &kv : bucket.ls) {
            if (kv.first == key) {
                value = kv.second;
                return true; // 键存在
            }
        }
        return false; // 键不存在
    }

    bool erase(const KeyType& key) {
        size_t index = hashFunction(key);
        auto &bucket = table[index];
        std::unique_lock<std::mutex> lk(bucket.mtx);
        for (auto it = bucket.ls.begin(); it != bucket.ls.end(); ++it) {
            if (it->first == key) {
                bucket.ls.erase(it);
                return true; 
            }
        }
        return false; // 键不存在
    }
};

int main() {
    HashMap<int, int> mp;
    for (int i = 0; i < 5; i++) {
        mp.insert(i, i * 10);
    }
    for (int i = 0; i < 5; i++) {
        int value = -1;
        if (mp.get(i, value)) {
            cout << i << " key find, value = " << value << endl;
        } else {
            cout << i << " key not find" << endl;
        }
    }
    for (int i = 0; i < 2; i++) {
        mp.erase(i);
    }
    cout << "==================" << endl;
    for (int i = 0; i < 5; i++) {
        int value = -1;
        if (mp.get(i, value)) {
            cout << i << " key find, value = " << value << endl;
        } else {
            cout << i << " key not find" << endl;
        }
    }
}
```

## 实现 string 类

问题：写一个简单的 String 类，包含构造函数，拷贝构造函数，移动构造函数，拷贝赋值函数，析构函数，通过通过指针初始化，在这个基础上实现一个 append 函数

实现思路：这个问题主要是想考查几种构造函数的写法，然后 append 的函数的话，考虑采用 STL vector 的思想，提前预留一定的空间，当添加的字符串长度超过这个空间的时候进行扩容，下面是我的代码实现，仅供参考

有一些注意点：  
- 1）采用 new 分配一个数组的时候，需要配套采用 delete[]，析构函数当中需要释放内存；
- 2）拷贝构造函数传递的参数是 const 引用类型，而移动构造函数不是引用类型；
- 3）拷贝、移动赋值运算符需要进行特判，防止自己给自己赋值导致对象失效的问题；

```cpp
#include <iostream>
#include <string.h>
using namespace std;

class String {
public:
  String() : str_(nullptr), size_(0), capacity_(10) {}
  String(const char *s) : capacity_(strlen(s) * 2), size_(strlen(s)) {
    str_ = new char[capacity_];
    strcpy(str_, s);
  }
  ~String() {
    if (str_) {
      delete[] str_;
    }
  }
  // 深拷贝
  String(const String &rhs) : size_(rhs.size_), capacity_(rhs.capacity_) {
    str_ = new char[rhs.size_];
    strcpy(str_, rhs.str_);
  }
  
  String& operator=(const String &rhs) {
    if (this == &rhs) {
        return *this;
    }
    str_ = new char[rhs.size_];
    strcpy(str_, rhs.str_);
    size_ = rhs.size_;
    capacity_ = rhs.capacity_;
    return *this;
  }

  String(String &&rhs) : str_(rhs.str_), size_(rhs.size_), capacity_(rhs.capacity_) {
    // 销毁rhs对象
    rhs.str_ = nullptr;
    rhs.capacity_ = 0;
    rhs.size_ = 0;
  }
  
  String& operator=(String &&rhs) {
    if (this == &rhs) {
        return *this;
    }
    str_ = rhs.str_;
    capacity_ = rhs.capacity_;
    size_ = rhs.size_;
    // 销毁rhs对象
    rhs.str_ = nullptr;
    rhs.capacity_ = 0;
    rhs.size_ = 0;
    return *this;
  }

  void append(const char *s) {
    int len = strlen(s);
    if (len + size_ < capacity_) {
      // 注意添加的字符串可能比扩容2倍后还要大，所以这里用循环判断一下
      while (len + size_ < capacity_) {
        capacity_ *= 2;
      }
      char *newstr = new char[capacity_ * 2];
      strcpy(newstr, str_);
      delete[] str_; // 删除旧的字符串
      str_ = newstr;
      size_ += len;
    } else {
      strcpy(str_ + size_, s);
      size_ += len;
    }
  }

  int size() const { return size_; }

  int capacity() const { return capacity_; }

  void print() { 
    if (str_) {
        cout << str_;
    }
    cout << endl;
  }

private:
  int capacity_ = 0;
  int size_ = 0;
  char *str_;
};

int main() {
  const char *s = "hello ";
  String str1(s);
  str1.print();
  str1.append("world! ");
  str1.print();
  str1.append("this is a massage");
  str1.print();

  std::cout << "===================" << std::endl;
  String tmp = "this is a string";
  String str2 = tmp; // 测试拷贝构造函数
  str2.print();

  String str3;
  str3 = str2; // 测试拷贝赋值运算符
  str3.print();

  String str4 = std::move(str2); // 测试移动构造函数
  str4.print();
  cout << "str2 = ";
  str2.print(); // str2应该为空

  cout << "test str5" << endl;
  String str5;
  str5 = std::move(str4); // 测试移动赋值运算符
  str5.print();
  
  return 0;
}
```

## 参考资料

- [线程池代码(整合版)](https://flowerhai.github.io/2023/04/18/thread-pool/thread-pool2/)
- [智能指针简单设计](https://flowerhai.github.io/2023/04/29/smart-pointers/ptr/)
- [C++面试之设计题](https://zhuanlan.zhihu.com/p/589795989)
- [单例模式 - 巴基速递](https://subingwen.cn/design-patterns/singleton/)