#include <cstdio>
#include "../include/logger.h"

using namespace mou;
using namespace mou::logger;

int main(void) {
    CHECK(0 == 1);
    CHECK_EQ(0, 1);
    CHECK_LT(0, 1);
    CHECK_GT(0, 1);
    CHECK_LE(0, 1);
    CHECK_GE(0, 1);
    return 0;
}
