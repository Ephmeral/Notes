#include <atomic>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <algorithm>
#include <numeric>
using namespace std;

class SimpleThreadPool {
public:
  explicit SimpleThreadPool(std::size_t n) {
    for (std::size_t i = 0; i < n; ++i) {
      std::thread{[this] {
        std::unique_lock<std::mutex> l(m_);
        while (true) {
          if (!q_.empty()) {
            auto task = std::move(q_.front());
            q_.pop();
            l.unlock();
            task();
            l.lock();
          } else if (done_) {
            break;
          } else {
            cv_.wait(l);
          }
        }
      }}.detach();
    }
  }

  ~SimpleThreadPool() {
    {
      std::lock_guard<std::mutex> l(m_);
      done_ = true; // cv_.wait 使用了 done_ 判断所以要加锁
    }
    cv_.notify_all();
  }

  template <typename F> void submit(F &&f) {
    {
      std::lock_guard<std::mutex> l(m_);
      q_.emplace(std::forward<F>(f));
    }
    cv_.notify_one();
  }

  void print_sum() {
    std::cout << "sum = " << sum << std::endl;
  }
private:
  std::mutex m_;
  std::condition_variable cv_;
  bool done_ = false;
  std::queue<std::function<void()>> q_;
  std::atomic_int sum;
};

std::vector<int> nums;
std::atomic_int sum;

int main() {
    for (int i = 0; i < nums.size(); ++i) {
        nums.push_back(i);
    }
    std::random_shuffle(nums.begin(), nums.end());

    SimpleThreadPool thread_pool(12);
    thread_pool.submit([&]() {
        for (int i = 0; i < nums.size() / 2; ++i) {
            sum += nums[i];
        }
    });
    thread_pool.submit([&]() {
        for (int i = nums.size() / 2; i < nums.size(); ++i) {
            sum += nums[i];
        }
    });
    std::cout << "accumulate = " << std::accumulate(nums.begin(), nums.end(), 0LL) << std::endl;
    thread_pool.print_sum();
    // std::cout << "sum = " << sum << std::endl;
}