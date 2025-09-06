#ifndef CODEUTILS_H
#define CODEUTILS_H

#include <stdlib.h>
#include <stdbool.h>

#define FREE(ptr) \
    if (ptr) \
    { \
        free(ptr); \
        ptr = NULL; \
    }

#define DELETE_LIST(list, deleter) \
    if (list) \
    { \
        deleteList(list, deleter); \
        list = NULL; \
    }

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

// a replacement for memset which is considered insecure by C11 standard (to be used for strings)
void setNChars(char* start, char value, size_t count);

char* createStringCopy(const char* source);
size_t copyNCharsToString(char* destination, const char* source, size_t count);

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
