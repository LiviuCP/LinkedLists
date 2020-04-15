#ifndef CODEUTILS_H
#define CODEUTILS_H

#include <stdlib.h>

typedef struct {
    int x;
    int y;
} Point;

#ifdef __cplusplus
extern "C"{
#endif

char* getLine();
int readUnsignedLong(size_t* number);
int isUnsignedLong(const char* input);

#ifdef __cplusplus
}
#endif

#endif // CODEUTILS_H
