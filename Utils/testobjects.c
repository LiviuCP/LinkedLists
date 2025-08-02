#include "testobjects.h"

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
        segment->start = (Point*)malloc(sizeof(Point));
        segment->stop = (Point*)malloc(sizeof(Point));

        if (segment->start != NULL && segment->stop != NULL)
        {
            segment->start->x = startX;
            segment->start->y = startY;
            segment->stop->x = stopX;
            segment->stop->y = stopY;
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

    return segment;
}

LocalConditions* createLocalConditionsPayload(int positionX, int positionY, int temperature, double humidity)
{
    LocalConditions* conditions = NULL;

    if (humidity >= 0.0 && humidity <= 100.0)
    {
        conditions = (LocalConditions*)malloc(sizeof(LocalConditions));

        if (conditions != NULL)
        {
            conditions->position = (Point*)malloc(sizeof(Point));

            if (conditions->position != NULL)
            {
                conditions->position->x = positionX;
                conditions->position->y = positionY;
                conditions->temperature = temperature;
                conditions->humidity = humidity;
            }
            else
            {
                free(conditions);
                conditions = NULL;
            }
        }
    }

    return conditions;
}

void emptyTestObject(Object* object)
{
    if (object != NULL)
    {
        if (object->payload != NULL)
        {
            if (object->type == SEGMENT)
            {
                Segment* segment = (Segment*)object->payload;
                free(segment->start);
                segment->start = NULL;
                free(segment->stop);
                segment->stop = NULL;
            }
            else if (object->type == LOCAL_CONDITIONS)
            {
                LocalConditions* conditions = (LocalConditions*)object->payload;
                free(conditions->position);
                conditions->position = NULL;
            }

            /* add further payloads here that have pointer members to dynamically allocated memory */

            object->type = -1;
            free(object->payload);
            object->payload = NULL;
        }
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
