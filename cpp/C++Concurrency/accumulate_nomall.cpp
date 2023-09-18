#include <iostream>
#include <algorithm>
#include <numeric>
#include <vector>
#include <thread>
using namespace std;

int main() {
    const int N = 10000000;
    vector<long long> nums(N, 0);
    for (int i = 0; i < N; ++i) {
        nums[i] = i;
    }

    std::random_shuffle(nums.begin(), nums.end());
    long long res = 0, init = 0;
    
    res = std::accumulate(nums.begin(), nums.end(), init);
    cout << "常规计算1-" << N << "的和为：" << res << endl;

    return 0;
}