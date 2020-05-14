#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#include "codeutils.h"

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

char* getLine()
{
    const size_t c_InitialBufferSize = 10;
    size_t bufferSize = c_InitialBufferSize;
    size_t currentPosition = 0;

    char* buffer = (char*)malloc(c_InitialBufferSize);

    if (buffer == NULL)
    {
        fprintf(stderr, "Bad memory allocation");
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
                fprintf(stderr, "Bad memory re-allocation");
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

boolean readUnsignedLong(size_t* number)
{
    char* input = getLine();
    boolean isValidInt = isUnsignedLong(input);

    if(isValidInt)
    {
        *number = (size_t)atoi(input);
    }

    free(input);
    input = NULL;

    return isValidInt;
}

boolean isUnsignedLong(const char *input)
{
    boolean isValid = TRUE;

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
                isValid = FALSE;
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
        isValid = FALSE;
    }

    return isValid;
}

boolean areDecimalNumbersEqual(double first, double second)
{
    const double epsilon = 1e-12;

    return (fabs(first - second) < epsilon);
}
