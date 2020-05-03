#include <string.h>

#include "testobjects.h"
#include "error.h"

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

void deleteTestObject(Object* object)
{
    if (object != NULL)
    {
        if (object->payload != NULL)
        {
            ASSERT_CONDITION(object->type != NULL, "Null object type when payload is not null"); // object should have both type and payload
            if (strcmp(object->type,"Segment") == 0)
            {
                Segment* segment = (Segment*)object->payload;
                free(segment->start);
                segment->start = NULL;
                free(segment->stop);
                segment->stop = NULL;
            }
            else if (strcmp(object->type, "LocalConditions") == 0)
            {
                LocalConditions* conditions = (LocalConditions*)object->payload;
                free(conditions->position);
                conditions->position = NULL;
            }
        }

        free(object->payload);
        object->payload = NULL;
        free(object->type);
        object->type = NULL;
        free(object);
        object = NULL;
    }
}
