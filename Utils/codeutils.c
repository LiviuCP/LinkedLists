#include <stdio.h>
#include <string.h>

#include "codeutils.h"

char *getLine()
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

int isUnsignedLong(const char *input)
{
    int isValid = 1;

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
                isValid = 0;
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
        isValid = 0;
    }

    return isValid;
}

int readUnsignedLong(size_t* number)
{
    char* input = getLine();
    int isValidInt = isUnsignedLong(input);

    if(isValidInt)
    {
        *number = (size_t)atoi(input);
    }

    free(input);
    input = NULL;

    return isValidInt;
}

Segment* createSegmentPayload(int startX, int startY, int stopX, int stopY)
{
    Segment* result = NULL;

    Segment* segment = (Segment*)malloc(sizeof(Segment));
    if (segment != NULL)
    {
        segment->start = (Point*)malloc(sizeof(Point));
        segment->stop = (Point*)malloc(sizeof(Point));

        if (segment->start != NULL && segment->stop != NULL)
        {
            segment->start->x = startX;
            segment->start->y = startY;
            segment->stop->x = stopX;
            segment->stop->y = stopY;
            result = segment;
        }
        else
        {
            free(segment->start);
            segment->start = NULL;
            free(segment->stop);
            segment->stop = NULL;
            free(segment);
            segment = NULL;
        }
    }

    return result;
}

LocalConditions* createLocalConditionsPayload(int positionX, int positionY, int temperature, double humidity)
{
    LocalConditions* result = NULL;

    if (humidity >= 0.0 && humidity <= 100.0)
    {
        LocalConditions* conditions = (LocalConditions*)malloc(sizeof(LocalConditions));

        if (conditions != NULL)
        {
            conditions->position = (Point*)malloc(sizeof(Point));
            if (conditions->position != NULL)
            {
                conditions->position->x = positionX;
                conditions->position->y = positionY;
                conditions->temperature = temperature;
                conditions->humidity = humidity;
                result = conditions;
            }
        }
    }

    return result;
}
