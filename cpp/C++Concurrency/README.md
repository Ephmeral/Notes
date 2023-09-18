# C++并发实战编程-笔记
## 1. 入门

```cpp
#include <iostream>
#include <thread>  // 1

void hello() { // 2
    std::cout << "Hello World!" << std::endl;
}

int main() {
    std::thread t(hello); // 3
    t.join();             // 4
    
    return 0;
}
```

- 1. 导入并发相关头文件
- 2. 新的线程开始的位置
- 3. 创建一个线程，新线程将 hello() 函数作为执行函数
- 4. 主线程 main 等待线程 t，否则可能出现主线程已经结束了，但是子线程还在运行的情况

# 2. 线程管理
## 2.1 线程基本操作

每个程序都有一个执行 main() 函数的线程，一个线程在创建会和 main 线程同时运行，线程执行完也会结束，同时创建一个线程对象后要等待这个线程结束。

### 2.1.1 启动线程

最简单的启动线程，就像上面的 hello 程序一样，传入一个无参数无返回值的函数，也可以通过传入函数对象、Lambda 表达式等

```cpp
// 传入函数
void do_some_work();
std::thread my_thread(do_some_work);

// 传入函数对象
class Background_task {
public:
	void operator()() const {
		do_some_thing();
		do_some_thing_else();
	}
};
Background_task fun;
std::thread my_thread2(fun);

// 传入Lambda表达式
std::thread my_thread3([](){
	do_some_thing();
	do_some_thing_else();
});
```

注意：如果传入临时的函数对象，可能会出现以下问题

```cpp
std::thread my_thread(Background_task()); // 这里被解析成函数声明了
// 相当于声明了my_thread函数，参数是一个函数指针（这个函数没有参数，返回值为Backgroud_task），返回值为std::thread
```

正确做法：

```cpp
std::thread my_thread((Background_task())); // 外面加一层括号
std::thread my_thread{Background_task()};   // 使用大括号统一初始化
```

线程启动后如果不等待线程结束，可能会出现问题，比如对象释放后还被访问，会产生未定义行为。

```cpp
#include <iostream>
#include <thread>

void do_something(int& i) {
    ++i;
    std::cout << i << std::endl;
}

struct func {
    int& i;

    func(int& i_): i(i_) {}

    void operator()() {
        for (unsigned j = 0; j < 1000000; ++j) {
            do_something(i);
        }
    }
};


void oops() {
    int some_local_state = 0;
    func my_func(some_local_state);
    std::thread my_thread(my_func);
    my_thread.detach();
    // my_thread.join();
}

int main() {
    oops();
    std::cout << "main is over" << std::endl;
    return 0;
}
```

上面代码运行的时候只会输出 `main is over`，do_some_thing 函数中对应 i 的值不会输出，应该是此时的 i 已经被销毁了，导致未知的错误（这个错误并不能从控制台输出体现出来）

### 2.1.2 等待线程完成

上面代码将 `my_thread.detach();` 替换为 `my_thread.join();` 即可正确输出 i 的值

调用 `join` 还会清理线程相关的内存，一旦使用过 `join` 线程将不能再次汇入了

### 2.1.3 特殊情况下等待

如果主程序运行的时候发生异常，可能会导致 `join` 函数没有正确运行，一种解决方法是进行异常处理

```cpp
void f() {
    int some_local_state=0;
    func my_func(some_local_state);
    std::thread t(my_func);
    try {
        do_something_in_current_thread();
    } catch(...) {
        t.join(); // 异常情况调用 join
        throw;
    }
    t.join();  // 正常情况调用 join
}
```

另外一种解决方法是使用 **RAII（资源获取即初始化，Resource Acquisition Is Initialization）**，提供一个类，在析构函数中调用 `join`

```cpp
class thread_guard {
    std::thread& t;
public:
	// 必须显式调用
    explicit thread_guard(std::thread& t_): t(t_) {}

    ~thread_guard() {
	    // 判断是否可join
        if(t.joinable()) {
            t.join();
        }
    }
    // 禁止拷贝成员
    thread_guard(thread_guard const&) = delete;
    thread_guard& operator=(thread_guard const&) = delete;
};

struct func; // 定义同上


void f() {
    int some_local_state = 0;
    func my_func(some_local_state);
    std::thread t(my_func);
    thread_guard g(t);
        
    do_something_in_current_thread();
}
```

`f()` 函数执行结束后，局部对象逆序被销毁，thread_guard 对象 g 第一个被销毁，此时 `join` 加入到原始线程中

### 2.1.4 后台运行线程

`detach()` 会使线程在后台运行，无法与主线程交互，分离线程通常称为 **守护线程**。分离线程之后也无法再汇入了

现有个文字处理程序，需要同时处理多个文档，一种方法是让每个文档处理程序拥有自己的线程，打开一个文档就启动一个线程。不同窗口之间相互独立，不需要等待其他线程完成。

```cpp
void edit_document(std::string const& filename) {
    open_document_and_display_gui(filename);
    while(!done_editing()) {
        user_command cmd = get_user_input();
        if(cmd.type == open_new_document) {
            std::string const new_name = get_filename_from_user();
            std::thread t(edit_document, new_name);
            t.detach(); // 打开新的文档，将这个线程分离出去
        } else {
            process_user_input(cmd);
        }
    }
}
```

## 2.2 传递参数

向可调用函数传入参数，可以直接在 `std::thread` 构造函数的后面附加上相应的参数即可，传入的参数会拷贝至线程空间中，无论是否为引用类型

```cpp
void f(int i, std::string const &s);

std::thread t(f, 3, "hello");
```

上面代码会将 `char const *` 类型的 `hello` 自动转为 `std::string`，如果传入的是指针变量可能会出现问题：

```cpp
void f(int i, std::string const &s);

void oops(int param) {
	char buff[1024];
	sprintf(buff, "%i", param);
	std::thread t(f, 3, buffer);
	t.detach();
}
```

函数 oops 可能会在 buffer 转为 `std::string` 之前结束，导致未定义的行为。因为无法保证隐式转换操作和 `std::thread` 构造函数的拷贝操作的顺序。解决方案是显式的将字符串字面值转为 `std::string`

```cpp
void f(int i, std::string const &s);

void oops(int param) {
	char buff[1024];
	sprintf(buff, "%i", param);
	std::thread t(f, 3, std::string(buffer)); // 避免悬空指针
	t.detach();
}
```

如果期待引用类型的参数，此时会出现编译错误，例如：

```cpp
void update_data_for_widget(widget_id w, widget_data &data);

void oops(widget_id w) {
	widget_data data;
	std::thread t(update_data_for_widget, w, data);
	t.join();
	process_widget_data(data);
}
```

`update_data_for_widget` 期待第二个参数是引用类型，但是 `std::thread` 会无视函数参数类型，会直接进行拷贝，拷贝的参数会以右值进行传递（便于支持移动的类型），而函数期望一个非常量引用类似（而非右值），此时编译报错。

解决方法是，通过 `std::bind` 中 `std::ref` 将参数转为引用的形式：

```cpp
std::thread t(update_data_for_widget, w, std::ref(data));
```

`std::thread` 和 `std::bind` 有类似的机制，可以传递一个成员函数指针作为线程函数，并提供一个合适的对象作为参数：

```cpp
#include <iostream>
#include <thread>

class X {
public:
    void do_something() {
        std::cout << "do something " << std::endl;
    }
    void do_something_else(int i) {
        std::cout << "do something else for " << i << std::endl;
    }
};

int main() {
    X my_x;
    std::thread t(&X::do_something, &my_x); // 无参数
    t.join();

    int num = 0;
    std::thread tt(&X::do_something_else, &my_x, num); // 传递一个参数
    tt.join();
    return 0;
}
```

新线程 t 会调用 `do_something()`，my_x 的地址作为对象指针提供给函数，第二种方式是加参数的写法，以此类推。

## 2.3 转移所有权

执行线程的所有权可以在 `std::thread` 中进行移动，下面是个例子：

```cpp
void some_function();
void some_other_function();
std::thread t1(some_function); //新的线程
std::thread t2 = std::move(t1); //t1所有权转移给t2

t1 = std::thread(some_other_function); // 创建一个thread临时对象，隐式将所有权转移给t1
std::thread t3;
t3 = std::move(t2); // 将t2关联的线程所有权转移给t3
t1 = std::move(t3); // 程序异常终止，t1已经有一个关联的线程了
```

在最后 系统会直接调用 `std::terminate()` 终止程序，为了保证线程的析构函数行为一致，前面提到线程对象析构前，会显示等待线程完成，或者分离，赋值的时候也需要如此，不能通过赋一个新的值来丢弃之前的线程。

`std::thread` 支持移动，线程的所有权可以在函数外进行转移：

```cpp
std::thread f() {
	void some_function();
	return std::thread(some_function);
}

std::thread g() {
	void some_other_function();
	std::thread t(some_other_function, 42);
	return t;
}
```

当所有权可以在函数内部传递，就可以使用 `std::thread` 作为参数传递：

```cpp
void f(std::thread t);

void g() {
	void some_function();
	f(std::thread(some_function));
	std::thread t(some_function);
	f(std::move(t));
}
```

`std::thread` 支持移动，可以用来创建 `thread_guard` 类，并且拥有其线程所有权，当某个对象转移线程所有权之后，就不能对它再进行加入或分离了：

```cpp
#include <thread>
#include <utility>

class scoped_thread {
    std::thread t;
public:
    explicit scoped_thread(std::thread t_):
        t(std::move(t_)) // 这里直接移动
    {
        if(!t.joinable())
            throw std::logic_error("No thread");
    }
    ~scoped_thread() {
        t.join();
    }
    scoped_thread(scoped_thread const&)=delete;
    scoped_thread& operator=(scoped_thread const&)=delete;
};

void do_something(int& i) {
    ++i;
}

struct func {
    int& i;

    func(int& i_):i(i_){}

    void operator()() {
        for(unsigned j=0;j<1000000;++j){
            do_something(i);
        }
    }
};

void do_something_in_current_thread(){}

void f(){
    int some_local_state;
    scoped_thread t(std::thread(func(some_local_state)));
        
    do_something_in_current_thread();
}

int main() {
    f();
}
```

- 主线程到达 f() 末尾时，scoped_thread 对象会被销毁，然后在析构函数会完成汇入
- 和 thread_guard 类不同的是，scoped_thread 在构造函数类进行判断是否 join，而 thread_guard 由于传入是引用类型，可能在主线程进行汇入，所以在析构函数中判断是否 join

`std::thread` 对象的容器，如果这个容器是移动敏感的 (比如，标准中的 `std::vector<>` )，那么移动操作同样适用于这些容器。下面代码量产了一些线程，并且等待它们结束。

```cpp
void f() {
    std::vector<std::thread> threads;
    for(unsigned i = 0;i < 20; ++i) {
        threads.push_back(std::thread(do_work,i)); // 产生线程
    }
    std::for_each(threads.begin(),threads.end(),
        std::mem_fn(&std::thread::join)); // 对每个线程调用join
}
```

## 2.4 运行时决定线程数量

`std::thread::hardware_concurrency()` 函数将返回 **能同时并发在一个程序中的线程数量**。例如，多核系统中，返回值可以是 CPU 核芯的数量。返回值仅作为一个提示，下面是一个代码示例：

```cpp
// accumulate_parallel.cpp
#include <iostream>
#include <algorithm>
#include <numeric>
#include <vector>
#include <thread>
using namespace std;

template<typename Iterator, typename T>
struct accumulate_block {
  void operator()(Iterator first, Iterator last, T& result) {
    result = std::accumulate(first, last, result);
  }
};

template<typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T init) {
  unsigned long const length = std::distance(first, last);

  if (!length) // 1 输入范围为空，返回init值
    return init;

  unsigned long const min_per_thread = 25;
  // 2 用范围内元素的总数量除以线程(块)中最小任务数，从而确定启动线程的最大数量
  unsigned long const max_threads =
      (length + min_per_thread -1) / min_per_thread; 

  // 这个函数将返回能同时并发在一个程序中的线程数量。
  unsigned long const hardware_threads =
      std::thread::hardware_concurrency(); 
  
  // 3 计算量的最大值和硬件支持线程数中，较小的值为启动线程的数量
  // 减少上下文切换的消耗
  unsigned long const num_threads= 
      std::min(hardware_threads != 0 ? hardware_threads : 2, max_threads);
  
  // 4 每个线程处理元素个数
  unsigned long const block_size = length / num_threads; 

  std::vector<T> results(num_threads); // 容器存放中间结果
  // 5 创建线程容器，比num_threads少一个，已经有一个主线程了
  std::vector<std::thread> threads(num_threads - 1);  

  Iterator block_start = first;
  // 下面代码依次为创建每个线程
  for(unsigned long i = 0; i < (num_threads - 1); ++i) {
    Iterator block_end = block_start;
    std::advance(block_end, block_size);  // 6 
    threads[i] = std::thread(     // 7
        accumulate_block<Iterator, T>(),
        block_start, block_end, std::ref(results[i]));
    block_start = block_end;  // 8
  }
  // 9 主线程处理最后结果，可能会有数据分配不一致，最后都由主线程进行处理
  accumulate_block<Iterator,T>()(
      block_start, last, results[num_threads - 1]); 
  // 10 等待所有线程
  std::for_each(threads.begin(), threads.end(),
       std::mem_fn(&std::thread::join));  
  // 11 累加最终结果
  return std::accumulate(results.begin(), results.end(), init); 
}

int main() {
    const int N = 10000000;
    vector<long long> nums(N, 0);
    for (int i = 0; i < N; ++i) {
        nums[i] = i;
    }

    std::random_shuffle(nums.begin(), nums.end());
    long long res = 0, init = 0;
    
    res = parallel_accumulate<std::vector<long long>::iterator, long long>(nums.begin(), nums.end(), init);
    cout << "并行计算1-" << N << "的和为：" << res << endl;

    return 0;
}
```

下面是做个简单的测试，发现效率上好像并没有加速多少，可能只是因为做简单的求和运算体现不出来。

```bash
$ time ./accumulate_parallel 
并行计算1-10000000的和为：49999995000000

real    0m1.670s
user    0m1.687s
sys     0m0.048s

$ time ./accumulate_nomall 
常规计算1-10000000的和为：49999995000000

real    0m1.721s
user    0m1.662s
sys     0m0.052s
```

## 2.5 识别线程

线程标识类型是 `std::thread::id`，可以通过两种方式进行检索：
- 第一种，可以通过调用 `std::thread` 对象的成员函数 `get_id()` 来直接获取。如果 `std::thread` 对象没有与任何执行线程相关联，`get_id()` 将返回 `std::thread::type` 默认构造值，这个值表示“没有线程”。
- 第二种，当前线程中调用 `std::this_thread::get_id()`(这个函数定义在 `<thread>` 头文件中) 也可以获得线程标识。

线程标识类型可以自由复制移动，标准库提供了比较等操作，可以拿来作为容器的元素进行排序等操作；标准库也提供 `std::hash<std::thread::id>` 容器，所以 `std::thread::id` 也可以作为无序容器的键值。

`std::thread::id` 实例常用作检测线程是否需要进行一些操作，比如：下面代码中可以通过 `std::this_thread::get_id()` 来获取线程标识从而区分主线程和其他线程，以做出不同的行为操作等。

```cpp
std::thread::id master_thread;
void some_core_part_of_algorithm() {
  if(std::this_thread::get_id() == master_thread) {
	do_master_thread_work();
  }
  do_common_work();
}
// 见代码this_thread_get_id.cpp
```

也可以通过流输出一个 `std::thread::id` 对象的值：

```cpp
std::cout<< std::this_thread::get_id();
```

# 3 线程间共享数据
## 3.1 共享数据带来的问题

当多个线程修改共享数据的时候，会产生很多问题。此时需要引入不变量的概念，下面是来自 gpt 的解释：

>在并发编程中，不变量（Invariant）是指在程序执行过程中始终保持不变的属性或条件。在并发编程中，特别是涉及多个线程或进程时，不变量非常重要，因为它们指示了程序需要维护的约束和前提条件，以确保程序的正确性和正确性。  
  例如，一个简单的不变量可能是一个排序数组的元素必须按照从小到大的顺序排列。这个不变量可以被用来验证数组是否被正确维护。如果一个线程在排序数组中插入一个元素而不遵循这个不变量，那么程序就可能陷入崩溃或出现 bug 的状态。

我的理解：这里可以将不变量看成一种状态，在代码执行的过程中要保持某种不变的状态，如果破坏了这个状态，就可能会导致出 bug。并发的时候需要考虑维护不变量。

### 3.1.1 条件竞争

不变量受到破坏的时候，会产生条件竞争。C++ 中定义了数据竞争：一种特殊的条件竞争，并发的修改一个独立对象，会导致未定义的行为。

并发对数据的条件竞争通常是恶性竞争：一般发生在对多个数据块进行修改，可能会出现一个线程正在修改一个数据块，另一个线程就访问这个数据块。

### 3.1.2 避免恶性条件竞争

1. 最简单的方法：对数据结构采用保护机制，对于不变量的修改，同一时刻最多只有一个。
2. 对数据结构和不变量修改，修改完的结构必须能完成一系列不可分割的变化，保证了每个不变量的状态，即所谓的无锁编程。
3. 用事务的方式去处理，需要的数据读取和存储都在事务日志，然后将之前一系列操作合并，再统一提交。

## 3.2 使用互斥量
### 3.2.1 互斥量

保护数据结构最基本的方式是，C++ 标准库提供的互斥量。

通过实例化 `std::mutex` 来创建一个互斥量，成员函数 `lock()` 对互斥量上锁，`unlock()` 解锁。推荐使用 C++ 标准库提供的 RAII 模版 `std::lock_guard`，在构造时提供互斥量上锁，析构时自动解锁，下面是个例子：

```cpp
// mutex_list.cpp
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
```

C++17 的一个新特性是模板类型推导，可以忽略 `std::lock_guard` 这类简单的模版类型，即下面代码也是可以的：

```cpp
std::lock_guard guard(some_mutex);
```

### 3.2.2 保护共享数据

只使用互斥量保护数据，也可能出现问题，例如可以通过指针或引用绕过这种保护，下面是将保护的数据作为运行时的参数的例子：

```cpp
class some_data {
    int a;
    std::string b;
public:
    void do_something() {}
};

class data_wrapper
{
private:
    some_data data;
    std::mutex m;
public:
    template<typename Function>
    void process_data(Function func)
    {
        std::lock_guard<std::mutex> l(m);
        func(data);
    }
};

some_data* unprotected;

void malicious_function(some_data& protected_data) {
    unprotected=&protected_data;
}

data_wrapper x;

void foo() {
    x.process_data(malicious_function); // 这里传入恶意函数
    unprotected->do_something(); // 绕过互斥量，无保护的情况下访问保护数据
}

int main() {
    foo();
}
```

### 3.2.3 接口间的条件竞争

实现一个容器的时候，不同的接口之间可能也存在竞争关系，例如下面实现的栈：

```cpp
template<typename T,typename Container=std::deque<T>>
class stack {
public:
    explicit stack(const Container&);
    explicit stack(Container&& = Container());
    template <class Alloc> explicit stack(const Alloc&);
    template <class Alloc> stack(const Container&, const Alloc&);
    template <class Alloc> stack(Container&&, const Alloc&);
    template <class Alloc> stack(stack&&, const Alloc&);

    bool empty() const;
    size_t size() const;
    T& top();
    T const& top() const;
    void push(T const&);
    void push(T&&);
    void pop();
    void swap(stack&&);
};
```

可能的问题是，当一个线程调用 empty 或 size 的时候，将返回值返回回去，此时另外一个线程调用了 push 或 pop 操作，这个时候调用 empty 或 size 的结果还是之前的，并没有在另外的线程调用 push 或 pop 后更新，导致出错。

可能的解决办法是：用互斥量保护内部的数据，保证同一个时刻只能有一个线程访问该数据。

### 3.2.4 死锁：问题描述和解决方案

死锁简单描述就是：两个线程都拥有一个互斥量，同时又在等待对方持有的互斥量，两个线程一直等待。

避免死锁简单的建议是：让两个以上互斥量按照相同的顺序上锁。在参数交换后，两个线程尝试同时交换两个数据，又会发生死锁。

`std::lock` 可以一次锁住多个锁，且不会发生死锁，下面是个示例：

```cpp
class some_big_object {};

void swap(some_big_object& lhs,some_big_object& rhs) {}

class X {
private:
    some_big_object some_detail;
    mutable std::mutex m;
public:
    X(some_big_object const& sd):some_detail(sd){}

    friend void swap(X& lhs, X& rhs) {
        if (&lhs==&rhs)
            return;
        std::lock(lhs.m,rhs.m); // 同时锁住两个锁
        //传入adopt_lock参数表示将锁交给lock_guard管理，而不需要构建新的锁了
        std::lock_guard<std::mutex> lock_a(lhs.m,std::adopt_lock);
        std::lock_guard<std::mutex> lock_b(rhs.m,std::adopt_lock);
        swap(lhs.some_detail,rhs.some_detail);
    }
};
```

C++17 提供了这种 RAII 支持 `std::scoped_lock`，和 `std::lock_guard` 类似，可以接收不定量的互斥量作为构造函数，下面是重构后的代码：

```cpp
void swap(X& lhs, X& rhs) {
	if (&lhs==&rhs)
		return;
	std::scoped_lock guard(lhs.m, rhs.m); // 这里同时用了C++17自动推导模版参数
	swap(lhs.some_detail,rhs.some_detail);
}
```

### 3.2.5 避免死锁的进阶指导

**1. 避免嵌套锁**：一个线程只获得一个锁

**2. 避免持有锁的时候调用外部代码**：因为没法确定外部代码会做什么样的操作，可能会额外的申请锁，导致死锁

**3. 使用固定顺序获得锁**：需要两个及以上锁的时候，最好通过固定的顺序来获得锁。

**4. 使用层次锁结构**

### 3.2.6 std::unique_lock——灵活的锁

```cpp
class some_big_object {};

void swap(some_big_object& lhs, some_big_object& rhs) {}

class X {
private:
    some_big_object some_detail;
    mutable std::mutex m;
public:
    X(some_big_object const& sd) : some_detail(sd) {}

    friend void swap(X& lhs, X& rhs) {
        if (&lhs == &rhs)
            return;
        std::unique_lock<std::mutex> lock_a(lhs.m, std::defer_lock);
        std::unique_lock<std::mutex> lock_b(rhs.m, std::defer_lock);
        std::lock(lock_a, lock_b); // 互斥量在这里上锁
        swap(lhs.some_detail, rhs.some_detail);
    }
};
```

`std::unique_lock` 传入第二个参数 std::defer_lock 表示互斥量应该保持解锁状态。

### 3.2.7 不同域中互斥量的传递

`std::unique_lock` 是可以移动的，但是不能赋值。互斥量的所有权可以通过移动操作，在不同的实例间进行传递。

```cpp
std::unique_lock<std::mutex> get_lock() {
	extern std::mutex some_mutex;
	std::unique_lock<std::mutex> lk(some_mutex);
	prepare_data(); // 准备数据
	return lk;
}

void process_data() {
	// 将get_lock的互斥量移动到另外一个实例中
	std::unique_lock<std::mutex> lk(get_lock()); 
	do_something();
}
```

- lk 在函数中不需要调用 `std::move()` 函数，会自动的调用移动构造函数；
- process_data() 函数中转移了 `std::unique_lock` 实例的所有权；

## 3.3 保护共享数据的方式
### 3.3.1 保护共享数据的初始化过程

现在有一个共享资源，只需要一次初始化就可以，这个时候需要考虑如何只进行一次初始化，下面是可能的一种写法：

```cpp
std::shared_ptr<some_resource> resource_ptr;
void foo() {
    if(!resource_ptr) {
        resource_ptr.reset(new some_resource); // 1
    }
    resource_ptr->do_something();
}
```

简单的说就是先看共享资源有没有内容（这里对应的是是否为空指针），如果没有分配资源，则进行初始化。

考虑多线程，可能有多个线程同时进入 1 的位置，这个时候会导致重复初始化，所以需要考虑并发，可以通过加锁来保护：

```cpp
std::shared_ptr<some_resource> resource_ptr;
std::mutex resource_mutex;
void foo() {
    std::unique_lock<std::mutex> lk(resource_mutex);
    if(!resource_ptr) {
        resource_ptr.reset(new some_resource); // 1
    }
    lk.unlock();
    resource_ptr->do_something();
}
```

这样的话，就不会有多个线程同时进入 1 的位置进行初始化了，再细粒度的加锁方式如下：

```cpp
std::shared_ptr<some_resource> resource_ptr;
std::mutex resource_mutex;
void foo() {
    if(!resource_ptr) {
	    std::unique_lock<std::mutex> lk(resource_mutex);
	    if (!resource_ptr)
	        resource_ptr.reset(new some_resource); // 1
    }
    lk.unlock();
    resource_ptr->do_something();
}
```

这也是著名的”双重检查锁模式“，书上说这里可能存在条件竞争？没想明白

C++ 标准库提供了 `std::once_flag` 和 `std::call_once` 来处理这种情况。

```cpp
std::shared_ptr<some_resource> resource_ptr;
std::once_flag resource_flag; // 1
void init_resource() {
	resource_ptr.reset(new some_resource);
}

void foo() {
	std::call_once(resource_flag,init_resource); // 可以完整的进行一次初始化
	resource_ptr->do_something();
}
```

### 3.3.2 保护不常更新的数据结构

C++17 提供了读写锁互斥量 `std::shared_mutex` 和 `std::shared_time_mutex`，下面是一个简单的例子：

```cpp
class dns_entry {};

class dns_cache {
    std::map<std::string, dns_entry> entries;
    std::shared_mutex entry_mutex;
public:
    dns_entry find_entry(std::string const& domain) {
        std::shared_lock<std::shared_mutex> lk(entry_mutex);
        std::map<std::string,dns_entry>::const_iterator const it=
            entries.find(domain);
        return (it==entries.end())?dns_entry():it->second;
    }
    
    void update_or_add_entry(std::string const& domain,
                             dns_entry const& dns_details) {
        std::lock_guard<std::shared_mutex> lk(entry_mutex);
        entries[domain]=dns_details;
    }
};
```

- 读操作通过 `std::shared_lock<std::shared_mutex>` 获取访问权；
- 而写操作可以通过 `std::lock_guard<std::shared_mutex>` 来进行保护；

### 3.3.3 嵌套锁

嵌套锁 `std::recursive_mutex` 可以对同一个实例获取多个锁，简单说就是一个线程多次获取同一个互斥量。

互斥量锁住其他线程前，必须释放拥有的所有锁，所以当调用 lock() 三次后，也必须调用 unlock() 三次。 `std::lock_guard<std::recursive_mutex>` 和 `std::unique_lock<std::recursive_mutex>` 可以解决这个问题。