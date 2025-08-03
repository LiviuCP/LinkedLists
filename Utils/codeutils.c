#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "codeutils.h"
#include "error.h"

Object* createObject(int type, void* payload)
{
    Object* result = NULL;

    if (payload != NULL)
    {
        Object* object = malloc(sizeof(Object));

        if (object != NULL)
        {
            object->type = type;
            object->payload = payload;
            result = object;
        }
    }

    return result;
}

void deleteObject(Object* object, void (*emptyObject)(Object* object))
{
    emptyObject(object);
    free(object);
    object = NULL;
}

void setNChars(char* start, char value, size_t count)
{
    if (start != NULL && count > 0)
    {
        for (size_t index = 0; index < count; ++index)
        {
            start[index] = value;
        }
    }
}

char* createStringCopy(const char* src)
{
    char* srcCopy = NULL;

    if (src)
    {
        const size_t requiredCharsCount = strlen(src) + 1;
        srcCopy = (char*)malloc(requiredCharsCount);

        if (srcCopy != NULL)
        {
            size_t index;

            for (index = 0; index < requiredCharsCount - 1; ++index)
            {
                srcCopy[index] = src[index];
            }

            srcCopy[index] = '\0';
        }
    }

    return srcCopy;
}

char* getLine()
{
    const size_t c_InitialBufferSize = 10;
    size_t bufferSize = c_InitialBufferSize;
    size_t currentPosition = 0;

    char* buffer = (char*)malloc(c_InitialBufferSize);

    if (buffer == NULL)
    {
        printf("Bad memory allocation");
        exit(-1);
    }

    for(;;)
    {
        if (currentPosition == bufferSize)
        {
            buffer = realloc(buffer, 2 * bufferSize);
            if (buffer != NULL)
            {
                bufferSize = bufferSize * 2;
            }
            else
            {
                printf("Bad memory re-allocation");
                exit(-1);
            }
        }

        char nextChar = (char)getc(stdin);

        if (nextChar == '\n')
        {
            buffer[currentPosition] = '\0';
            break;
        }
        else
        {
            buffer[currentPosition] = nextChar;
            ++currentPosition;
        }
    }

    return buffer;
}

bool readUnsignedLong(size_t* number)
{
    char* input = getLine();
    bool isValidInt = isUnsignedLong(input);

    if (isValidInt)
    {
        *number = (size_t)atoi(input);
    }

    free(input);
    input = NULL;

    return isValidInt;
}

bool isUnsignedLong(const char *input)
{
    bool isValid = true;

    if (input != NULL && strlen(input) != 0)
    {
        size_t currentPos = 0;

        while (input[currentPos] != '\0')
        {
            switch(input[currentPos])
            {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                break;
            default:
                isValid = false;
            }

            ++currentPos;

            if (!isValid)
            {
                break;
            }
        }
    }
    else
    {
        isValid = false;
    }

    return isValid;
}

bool convertIntToString(int valueToConvert, char* str, size_t availableCharsCount)
{
    bool result = false;
    const size_t minRequiredCharsCount = sizeof(int) <= 4 ? 12 : 21; // possible sign and terminating character included

    if (str != NULL && availableCharsCount >= minRequiredCharsCount)
    {
        size_t currentCharIndex = 0;
        const char asciiChar0 = '0';

        setNChars(str, '\0', availableCharsCount);

        if (valueToConvert < 0)
        {
            str[currentCharIndex] = '-';
            valueToConvert = -valueToConvert;
            ++currentCharIndex;
        }

        char convertedDigitChars[availableCharsCount];
        size_t convertedDigitCharsCount = 0;

        int quotient = valueToConvert;
        int remainder = 0;

        do
        {
            remainder = quotient % 10;
            quotient = quotient / 10;
            convertedDigitChars[convertedDigitCharsCount] = (char)(asciiChar0 + remainder);
            ++convertedDigitCharsCount;
        }
        while (quotient > 0);

        ASSERT(convertedDigitCharsCount > 0, "At least one char should have been found for the integer!");

        // enter the digit chars within result string in reverse order of their "discovery"
        for (size_t charsToReverseCount = convertedDigitCharsCount; charsToReverseCount > 0; --charsToReverseCount)
        {
            str[currentCharIndex] = convertedDigitChars[charsToReverseCount - 1];
            ++currentCharIndex;
        }

        result = true;
    }

    return result;
}

bool areDecimalNumbersEqual(double first, double second)
{
    const double epsilon = 1e-12;

    return (fabs(first - second) < epsilon);
}

void clearScreen()
{
#if defined (UNIX_OS)
    system("clear"); // Linux & Mac
#elif defined(_WIN32)
    system("cls"); // Windows
#endif
}
