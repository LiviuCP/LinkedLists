#ifndef CODEUTILS_H
#define CODEUTILS_H

#include <stdlib.h>

typedef unsigned int boolean;

#define TRUE 1u
#define FALSE 0u

typedef struct
{
    int type;
    void* payload;
}
Object;

#ifdef __cplusplus
extern "C"{
#endif

Object* createObject(int type, void* payload);
void deleteObject(Object* object, void (*emptyObject)(Object* object));

char* getLine();
boolean readUnsignedLong(size_t* number);

boolean isUnsignedLong(const char* input);
boolean areDecimalNumbersEqual(double first, double second);

#ifdef __cplusplus
}
#endif

#endif // CODEUTILS_H
