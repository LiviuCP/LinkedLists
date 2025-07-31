#ifndef ERROR_H
#define ERROR_H

#include <assert.h>

#ifndef NDEBUG
#define ASSERT(condition, message)                                              \
        {                                                                       \
            if(!(condition))                                                    \
            {                                                                   \
                printf("ASSERTION ERROR! ");                                    \
                printf(message);                                                \
                printf("\n");                                                   \
                assert(false);                                                  \
            }                                                                   \
        }
#else
#define ASSERT(condition, message)
#endif

#endif // ERROR_H
