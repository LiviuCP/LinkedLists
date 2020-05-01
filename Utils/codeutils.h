#ifndef CODEUTILS_H
#define CODEUTILS_H

#include <stdlib.h>

typedef unsigned int boolean;

#define TRUE 1u
#define FALSE 0u

typedef struct
{
    char* type;
    void* payload;
}
Object;

#ifdef __cplusplus
extern "C"{
#endif

char* getLine();
boolean readUnsignedLong(size_t* number);
boolean isUnsignedLong(const char* input);

#ifdef __cplusplus
}
#endif

#endif // CODEUTILS_H
