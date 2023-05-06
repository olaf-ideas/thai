#ifndef THAI_UTILS_H
#define THAI_UTILS_H

namespace thai_poker {

int pop_count(unsigned int v) {
    return __builtin_popcount(v);
}

}

#endif // THAI_UTILS_H