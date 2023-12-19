#include <bits/stdc++.h>
using namespace std;

const int N = 1e5 + 10;
int primes[N], cnt = 0;
bool st[N];

void get_primes(int n) {
    for (int i = 2; i <= n; ++i) {
        if (!st[i]) {
            primes[cnt++] = i;
            for (int j = i; j <= n; j += i) {
                st[j] = true;
            }
        }
    }
}

void get_primes2(int n) {
    for (int i = 2; i <= n; ++i) {
        if (!st[i]) primes[cnt++] = i;
        for (int j = 0; primes[j] <= n / i; ++j) {
            st[primes[j] * i] = true;
            if (i % primes[j] == 0) {
                break;
            }
        }
    }
}

int main() {
    ios::sync_with_stdio(false); cin.tie(0); cout.tie(0);
    int n; cin >> n;
    get_primes2(n);
    for (int i = 0; i < cnt; ++i) {
        cout << primes[i] << endl;
    }
    return 0;
}