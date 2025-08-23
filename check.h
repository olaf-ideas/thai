#ifndef THAI_CHECK_H
#define THAI_CHECK_H

#include <cassert>
#include <cstdio>

#include "const.h"
#include "utils.h"


namespace thai_poker {

namespace check {

bool satisfies_bet(Hand h, Bet b) {
    assert(b != Bet::CHECK);
}

}

}

#endif // THAI_CHECK_H