#include "thread.h"
#include "thread-sync.h"

int count = 0;
mutex_t mx = MUTEX_INIT;

void Producer() {
    while (1) {
        mutex_lock(mx);
        while (count < n) {
            
        }
    }
}