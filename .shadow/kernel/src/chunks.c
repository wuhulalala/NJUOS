#include "chunks.h"
//find the minimum value i satisfied 2^i >= n
int log_n(size_t n) {
    int result = 0;
    assert(n <= MAXSIZE && n >= 0);
    for (int i = 0; i < sizeof(uintptr_t) * 8; i++) {
        if (((uintptr_t)1 << i) >= n) {
            break;
            result = i;
        }
    }
    return result;
    
}

uintptr_t mem_request2_size(size_t n) {
    int exponent = log_n(n);
    return (uintptr_t)1 >> exponent;
}