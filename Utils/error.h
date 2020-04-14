#ifndef ERROR_H
#define ERROR_H

#include <stdio.h>
#include <assert.h>

#ifndef NDEBUG
#define ASSERT_CONDITION(condition, message)                                    \
        {                                                                       \
            if(!(condition))                                                    \
            {                                                                   \
                fprintf(stderr, "ASSERTION ERROR! ");                           \
                fprintf(stderr, message);                                       \
                fprintf(stderr, "\n");                                          \
                assert(0);                                                      \
            }                                                                   \
        }
#else
#define ASSERT_CONDITION(condition, message)
#endif

#endif // ERROR_H
