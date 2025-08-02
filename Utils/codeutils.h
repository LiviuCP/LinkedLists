#ifndef CODEUTILS_H
#define CODEUTILS_H

#include <stdlib.h>
#include <stdbool.h>

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

char* createStringCopy(const char* src);

char* getLine();
bool readUnsignedLong(size_t* number);

bool isUnsignedLong(const char* input);
bool convertIntToString(int valueToConvert, char* str, size_t availableCharsCount);
bool areDecimalNumbersEqual(double first, double second);

void clearScreen();

#ifdef __cplusplus
}
#endif

#endif // CODEUTILS_H
