#ifndef MOU_LOGGER_H
#define MOU_LOGGER_H

#include <cstdio>

namespace mou {

namespace logger {

#define CHECK(state) \
    if (!(state)) { \
        printf("%s:%d Check %s failed\n", __FILE__, __LINE__, #state);\
    }

#define CHECK_EQ(a, b) \
    if (!((a) == (b))) { \
        printf("%s:%d Check %s == %s failed\n", __FILE__, __LINE__, #a, #b); \
    }

#define CHECK_LT(a, b) \
    if (!((a) < (b))) { \
        printf("%s:%d Check %s < %s failed\n", __FILE__, __LINE__, #a, #b); \
    }

#define CHECK_GT(a, b) \
    if (!((a) > (b))) { \
        printf("%s:%d Check %s > %s failed\n", __FILE__, __LINE__, #a, #b); \
    }

#define CHECK_LE(a, b) \
    if (!((a) <= (b))) { \
        printf("%s:%d Check %s <= %s failed\n", __FILE__, __LINE__, #a, #b); \
    }

#define CHECK_GE(a, b) \
    if (!((a) >= (b))) { \
        printf("%s:%d Check %s >= %s failed\n", __FILE__, __LINE__, #a, #b); \
    }

} // namespace logger

} // namespace mou

#endif // MOU_LOGGER_H
