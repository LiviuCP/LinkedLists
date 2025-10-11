#include "testobjects.h"
#include "error.h"

#include <stdio.h>

int* createIntegerPayload(int value)
{
    int* integer = (int*)malloc(sizeof(int));

    if (integer != NULL)
    {
        *integer = value;
    }

    return integer;
}

double* createDecimalPayload(double value)
{
    double* decimal = (double*)malloc(sizeof(double));

    if (decimal != NULL)
    {
        *decimal = value;
    }

    return decimal;
}

Point* createPointPayload(int x, int y)
{
    Point* point = (Point*)malloc(sizeof(Point));

    if (point != NULL)
    {
        point->x = x;
        point->y = y;
    }

    return point;
}

Segment* createSegmentPayload(int startX, int startY, int stopX, int stopY)
{
    Segment* segment = (Segment*)malloc(sizeof(Segment));

    if (segment != NULL)
    {
        segment->start.x = startX;
        segment->start.y = startY;
        segment->stop.x = stopX;
        segment->stop.y = stopY;
    }

    return segment;
}

LocalConditions* createLocalConditionsPayload(int positionX, int positionY, int temperature, double humidity)
{
    LocalConditions* conditions = NULL;

    const double minHumidity = 0.0;
    const double maxHumidity = 100.0;

    if (humidity >= minHumidity && humidity <= maxHumidity)
    {
        conditions = (LocalConditions*)malloc(sizeof(LocalConditions));

        if (conditions != NULL)
        {
            conditions->position.x = positionX;
            conditions->position.y = positionY;
            conditions->temperature = temperature;
            conditions->humidity = humidity;
        }
    }

    return conditions;
}

void emptyTestObject(Object* object)
{
    if (object != NULL && object->payload != NULL)
    {
        switch (object->type) {
        case INTEGER:
        case DECIMAL:
        case POINT:
        case SEGMENT:
        case LOCAL_CONDITIONS:
            break;
        /* for new payload types: include the "custom" code for freeing memory here if they have pointer members to dynamically allocated memory */
        default:
            ASSERT(false, "Invalid test object type!");
        }

        object->type = -1;
        free(object->payload);
        object->payload = NULL;
    }
}

const char* getTestObjectTypeAsString(int type)
{
    const char* result;

    switch(type)
    {
    case INTEGER:
        result = "integer";
        break;
    case DECIMAL:
        result = "decimal";
        break;
    case POINT:
        result = "point";
        break;
    case SEGMENT:
        result = "segment";
        break;
    case LOCAL_CONDITIONS:
        result = "local conditions";
        break;
    default:
        result = "unknown";
    }

    return result;
}
