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