#pragma once

#include <assert.h>

#ifndef NDEBUG
#define ASSERT(condition, message)                                                                                     \
    {                                                                                                                  \
        if (!(condition))                                                                                              \
        {                                                                                                              \
            printf("ASSERTION ERROR! ");                                                                               \
            printf(message);                                                                                           \
            printf("\n");                                                                                              \
            assert(false);                                                                                             \
        }                                                                                                              \
    }
#else
#define ASSERT(condition, message)
#endif
