#ifndef CODEUTILS_H
#define CODEUTILS_H

#include <stdlib.h>

typedef unsigned int boolean;

#define TRUE 1u
#define FALSE 0u

// used for restricting part of the code from compiling onto non-UNIX systems when containing POSIX threads (pthread.h)
#if defined (__unix__) || (defined (__APPLE__) && defined (__MACH__))
#define UNIX_OS
#endif

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

void clearScreen();

#ifdef __cplusplus
}
#endif

#endif // CODEUTILS_H
