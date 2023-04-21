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

