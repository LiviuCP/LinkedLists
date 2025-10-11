#include "listelement.h"
#include "testobjects.h"
#include "error.h"

#include <stdio.h>

ListElement* createListElement()
{
    ListElement* result = (ListElement*)malloc(sizeof(ListElement));

    if (result != NULL)
    {
        initListElement(result);
    }

    return result;
}

void initListElement(ListElement* element)
{
    if (element != NULL)
    {
        element->next = NULL;
        element->priority = 0;

        // an empty object should have NULL payload and a negative type
        element->object.type = -1;
        element->object.payload = NULL;
    }
}

void assignObjectContentToListElement(ListElement* element, const int objectType, void* const objectPayload)
{
    if (element != NULL)
    {
        ASSERT(objectPayload == NULL || objectType >= 0, "Attempt to assign object that has a type inconsistent with the payload!")
        ASSERT(element->object.payload == NULL, "Attempt to assign object without emptying the existing one first!");

        element->object.type = objectType;
        element->object.payload = objectPayload;
    }
}

Object detachContentFromListElement(ListElement* element)
{
    Object result;
    result.type = -1;
    result.payload = NULL;

    if (element != NULL)
    {
        result.type = element->object.type;
        result.payload = element->object.payload;
        element->object.type = -1;
        element->object.payload = NULL;
    }

    return result;
}

// user should pass a custom deleter for objects with payloads containing pointers to allocated heap memory
void deleteObjectPayload(Object *object)
{
    if (object != NULL)
    {
        FREE(object->payload);
        object->type = -1;
    }
}

/* This function is just added for having a default value to be passed to the copyContentToList() function as deep copying function pointer.
   User is responsible to pass a custom deep copying function with this signature if any list element contains an non-empty Object.

   ---> To be used for lists with EMPTY objects only!
*/
bool copyObjectPlaceholder(const ListElement* source, ListElement* destination)
{
    bool success = false;

    if (source != NULL && destination != NULL)
    {
        ASSERT(source->object.type < 0 && source->object.payload == NULL && destination->object.type < 0 && destination->object.payload == NULL,
                         "The source and/or destination element object is either invalid or non-empty");
        success = true;
    }

    return success;
}

/* This function is just for illustrating the creation of a custom function for deep copying an Object
    ---> for test purposes only
*/
bool customCopyObject(const ListElement* source, ListElement* destination)
{
    bool success = false;

    ASSERT(source->object.type >= 0, "Invalid source object type!");

    /* copy a NON-EMPTY source object to EMPTY destination object to avoid any memory leaks */
    if (source != NULL && destination != NULL && source->object.type >= 0 && source->object.payload != NULL && destination->object.payload == NULL)
    {
        void* destinationObjectPayload = NULL;

        switch(source->object.type)
        {
        case INTEGER: {
            const int* sourceInteger = (int*)source->object.payload;
            destinationObjectPayload = createIntegerPayload(*sourceInteger);
            break;
        }
        case DECIMAL: {
            const double* sourceDecimal = (double*)source->object.payload;
            destinationObjectPayload = createDecimalPayload(*sourceDecimal);
            break;
        }
        case POINT: {
            const Point* sourcePoint = (Point*)source->object.payload;
            destinationObjectPayload = createPointPayload(sourcePoint->x, sourcePoint->y);
            break;
        }
        case SEGMENT: {
            const Segment* sourceSegment = (Segment*)source->object.payload;
            destinationObjectPayload = createSegmentPayload(sourceSegment->start.x, sourceSegment->start.y, sourceSegment->stop.x, sourceSegment->stop.y);
            break;
        }
        case LOCAL_CONDITIONS: {
            const LocalConditions* localConditions = (LocalConditions*)source->object.payload;
            destinationObjectPayload = createLocalConditionsPayload(localConditions->position.x,
                                                                    localConditions->position.y,
                                                                    localConditions->temperature,
                                                                    localConditions->humidity);
            break;
        }
        default:
            ASSERT(false, "Unhandled object type!");
            break;
        }

        destination->object.type = destinationObjectPayload != NULL ? source->object.type : -1;
        destination->object.payload = destinationObjectPayload;

        success = destination->object.type >= 0;
    }

    return success;
}
