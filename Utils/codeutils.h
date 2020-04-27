#ifndef CODEUTILS_H
#define CODEUTILS_H

#include <stdlib.h>

typedef unsigned int boolean;

#define TRUE 1u
#define FALSE 0u

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
