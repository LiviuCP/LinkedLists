#include <stdio.h>

#include "listelement.h"
#include "bitoperations.h"
#include "testobjects.h"
#include "error.h"

typedef struct
{
    ListElement* elements;
    ListElement** elementRefs;
    byte_t* availabilityFlags;
    size_t totalCount;
    size_t availableCount;
} ListElementsPoolContent;

// "private" (supporting) functions
static bool initListElementsPool(ListElementsPool* elementsPool);
static void cleanupListElementsPool(ListElementsPool* elementsPool);

ListElement* createListElement()
{
    ListElement* result = (ListElement*)malloc(sizeof(ListElement));

    if (result != NULL)
    {
        result->next = NULL;
        result->priority = 0;

        // an empty object should have NULL payload and a negative type
        result->object.type = -1;
        result->object.payload = NULL;
    }

    return result;
}

ListElementsPool* createListElementsPool()
{
    ListElementsPool* elementsPool = (ListElementsPool*)malloc(sizeof(ListElementsPool));

    if (elementsPool != NULL)
    {
        const bool success = initListElementsPool(elementsPool);

        if (!success)
        {
            free(elementsPool);
            elementsPool = NULL;
        }
    }

    return elementsPool;
}

void deleteListElementsPool(ListElementsPool* elementsPool)
{
    if (elementsPool != NULL)
    {
        cleanupListElementsPool(elementsPool);
        free(elementsPool);
        elementsPool = NULL;
    }
}

size_t getAvailableElementsCount(ListElementsPool* elementsPool)
{
    size_t count = 0;

    if (elementsPool != NULL)
    {
        ListElementsPoolContent* content = (ListElementsPoolContent*)elementsPool->content;
        ASSERT(content != NULL, "Invalid list elements elementsPool content!");

        if (content != NULL)
        {
            count = content->availableCount;
        }
    }

    return count;
}

/* TODO:
   - implement mechanism for additional allocations of elements in case all elements are assigned and further requests are made
   - implement mechanism for allocation of element batches (e.g. when initializing a list with multiple elements)
*/
ListElement* aquireElement(ListElementsPool* elementsPool)
{
    ListElement* aquiredElement = NULL;

    if (elementsPool != NULL)
    {
        ListElementsPoolContent* content = (ListElementsPoolContent*)elementsPool->content;
        ASSERT(content != NULL, "Invalid list elements pool content!");

        if (content != NULL && content->availableCount > 0)
        {
            ListElement** elementRefs = content->elementRefs;
            byte_t* availabilityFlags = content->availabilityFlags;

            ASSERT(elementRefs != NULL, "Invalid list elements pool data refs!");
            ASSERT(availabilityFlags != NULL, "Invalid list elements pool availability flags!");

            if (elementRefs != NULL && availabilityFlags != NULL)
            {
                const size_t elementRefIndex = content->availableCount - 1;

                aquiredElement = elementRefs[elementRefIndex];
                elementRefs[elementRefIndex] = NULL;
                --content->availableCount;

                const ListElement* firstElement = content->elements;

                if (aquiredElement >= firstElement && aquiredElement < firstElement + content->totalCount)
                {
                    const size_t elementIndex = aquiredElement - firstElement;
                    const size_t byteIndex = elementIndex / BYTE_SIZE;
                    const size_t bitIndex = BYTE_SIZE - 1 - elementIndex % BYTE_SIZE; // bits are numbered from byte end (least significant: 0) to beginning (most significant: 7)
                    const byte_t elementBitMask = LSB_MASK << bitIndex;
                    availabilityFlags[byteIndex] &= ~elementBitMask; // bit of the element set to 0, element is aquired (hence unavailable)
                }
                else
                {
                    ASSERT(false, "Invalid element address detected!");
                }
            }
        }
    }

    return aquiredElement;
}

bool releaseElement(ListElement* element, ListElementsPool* elementsPool)
{
    bool success = false;

    if (elementsPool != NULL && element != NULL)
    {
        ListElementsPoolContent* content = (ListElementsPoolContent*)elementsPool->content;

        const bool isValidPool = content != NULL &&
                                 content->elements != NULL &&
                                 content->elementRefs != NULL &&
                                 content->availabilityFlags != NULL &&
                                 content->totalCount > 0 &&
                                 content->totalCount >= content->availableCount;

        ASSERT(isValidPool, "Invalid list elements elementsPool parameters!");

        const ListElement* firstElement = content->elements;

        if (isValidPool && element >= firstElement && element < firstElement + content->totalCount)
        {
            const size_t elementIndex = element - firstElement;
            const size_t byteIndex = elementIndex / BYTE_SIZE;
            const size_t bitIndex = BYTE_SIZE - 1 - elementIndex % BYTE_SIZE; // bits are numbered from byte end (least significant: 0) to beginning (most significant: 7)
            const byte_t elementBitMask = LSB_MASK << bitIndex;
            const bool isElementAvailableInPool = content->availabilityFlags[byteIndex] & elementBitMask; // test element availability bit

            // only "unavailable" (aquired) elements can be released
            if (!isElementAvailableInPool)
            {
                ASSERT(content->availableCount < content->totalCount, "Count of available elements should be lower than total!");

                element->next = NULL;
                element->object.type = -1;
                element->object.payload = NULL;
                element->priority = 0;
                content->elementRefs[content->availableCount] = element;
                content->availabilityFlags[byteIndex] |= elementBitMask; // set bit, element is available again for aquiring
                ++content->availableCount;
                success = true;
            }
        }
    }

    return success;
}

void assignObjectContentToListElement(ListElement* element, const int objectType, void* const objectPayload)
{
    if (element != NULL && objectPayload != NULL && objectType >= 0)
    {
        ASSERT(element->object.payload == NULL, "Attempt to assign object without emptying the existing one first!");

        element->object.type = objectType;
        element->object.payload = objectPayload;
    }
}

Object* detachContentFromListElement(ListElement* element)
{
    Object* result = NULL;

    if (element != NULL)
    {
        ASSERT((element->object.type >= 0 && element->object.payload != NULL) || (element->object.type < 0 && element->object.payload == NULL), "Invalid object detected");

        Object* temp = createObject(element->object.type, element->object.payload);

        if (temp != NULL)
        {
            result = temp;
            element->object.type = -1;
            element->object.payload = NULL;
        }
    }

    return result;
}

// user should pass a custom deleter for objects with payloads containing pointers to allocated heap memory
void deleteObjectPayload(Object *object)
{
    if (object != NULL)
    {
        free(object->payload);
        object->payload = NULL;
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

/* This function is just for illustrating the creation of custom deep copy function
    ---> for test purposes only
*/
bool customCopyObject(const ListElement* source, ListElement* destination)
{
    bool success = false;

    /* copy a NON-EMPTY source object to EMPTY destination object to avoid any memory leaks */
    if (source != NULL && destination != NULL && source->object.payload != NULL && destination->object.payload == NULL)
    {
        ASSERT(source->object.type >= 0, "Invalid source object detected");

        if (source->object.type == SEGMENT)
        {
            Segment* destinationObjectPayload = (Segment*)malloc(sizeof(Segment));
            if (destinationObjectPayload != NULL)
            {
                destinationObjectPayload->start = (Point*)malloc(sizeof(Point));
                destinationObjectPayload->stop = (Point*)malloc(sizeof(Point));

                if (destinationObjectPayload->start != NULL && destinationObjectPayload->stop != NULL)
                {
                    destinationObjectPayload->start->x = ((Segment*)source->object.payload)->start->x;
                    destinationObjectPayload->start->y = ((Segment*)source->object.payload)->start->y;
                    destinationObjectPayload->stop->x = ((Segment*)source->object.payload)->stop->x;
                    destinationObjectPayload->stop->y = ((Segment*)source->object.payload)->stop->y;
                    destination->object.type = source->object.type;
                    destination->object.payload = (void*)destinationObjectPayload;
                    success = true;
                }

                if (!success) // ensure any allocated memory is freed in case deep copy failed
                {
                    free(destinationObjectPayload->start);
                    destinationObjectPayload->start = NULL;
                    free(destinationObjectPayload->stop);
                    destinationObjectPayload->stop = NULL;
                    free(destinationObjectPayload);
                    destinationObjectPayload = NULL;
                }
            }
        }
        else if (source->object.type == LOCAL_CONDITIONS)
        {
            LocalConditions* destinationObjectPayload = (LocalConditions*)malloc(sizeof(LocalConditions));
            if (destinationObjectPayload != NULL)
            {
                destinationObjectPayload->position = (Point*)malloc(sizeof(Point));
                if (destinationObjectPayload->position != NULL)
                {
                    destinationObjectPayload->position->x = ((LocalConditions*)source->object.payload)->position->x;
                    destinationObjectPayload->position->y = ((LocalConditions*)source->object.payload)->position->y;
                    destinationObjectPayload->humidity = ((LocalConditions*)source->object.payload)->humidity;
                    destinationObjectPayload->temperature = ((LocalConditions*)source->object.payload)->temperature;
                    destination->object.type = source->object.type;
                    destination->object.payload = (void*)destinationObjectPayload;
                    success = true;
                }

                if (!success) // ensure any allocated memory is freed in case deep copy failed
                {
                    free(destinationObjectPayload);
                    destinationObjectPayload = NULL;
                }
            }
        }
    }

    return success;
}

static bool initListElementsPool(ListElementsPool* elementsPool)
{
    bool success = false;

    ListElementsPoolContent* content = NULL;
    ListElement* elements = NULL;
    ListElement** elementRefs = NULL;
    byte_t* availabilityFlags = NULL;
    const size_t totalCount = MAX_POOLED_ELEMENTS_COUNT;
    const size_t availabilityBytesCount = totalCount / BYTE_SIZE + (totalCount % BYTE_SIZE > 0 ? 1 : 0);

    if (elementsPool != NULL)
    {
        content = malloc(sizeof(ListElementsPoolContent));
    }

    if (content != NULL)
    {
        elements = (ListElement*)malloc(totalCount * sizeof(ListElement));
    }

    if (elements != NULL)
    {
        elementRefs = (ListElement**)malloc(totalCount * sizeof(ListElement*));
    }

    if (elementRefs != NULL)
    {
        availabilityFlags = (byte_t*)malloc(availabilityBytesCount);
    }

    if (availabilityFlags != NULL)
    {
        success = true;

        for (size_t index = 0; index < totalCount; ++index)
        {
            elements[index].next = NULL;
            elements[index].object.type = -1;
            elements[index].object.payload = NULL;
            elements[index].priority = 0;
            elementRefs[index] = &elements[index];
        }

        for (size_t index = 0; index < availabilityBytesCount; ++index)
        {
            availabilityFlags[index] = MAX_BYTE_VALUE;
        }

        content->elements = elements;
        content->elementRefs = elementRefs;
        content->availabilityFlags = availabilityFlags;
        content->totalCount = totalCount;
        ASSERT(content->totalCount > 0, "Total pooled elements count should not be 0!");
        content->availableCount = content->totalCount;
        elementsPool->content = content;
    }

    if (!success)
    {
        if (content != NULL)
        {
            free(content);
            content = NULL;
        }

        if (elements != NULL)
        {
            free(elements);
            elements = NULL;
        }

        if (elementRefs != NULL)
        {
            free(elementRefs);
            elementRefs = NULL;
        }

        if (availabilityFlags != NULL)
        {
            free(availabilityFlags);
            availabilityFlags = NULL;
        }
    }

    return success;
}

static void cleanupListElementsPool(ListElementsPool* elementsPool)
{
    if (elementsPool != NULL)
    {
        ListElementsPoolContent* content = (ListElementsPoolContent*)elementsPool->content;

        if (content != NULL)
        {
            if (content->elements != NULL)
            {
                free(content->elements);
                content->elements = NULL;
            }

            if (content->elementRefs != NULL)
            {
                free(content->elementRefs);
                content->elementRefs = NULL;
            }

            if (content->availabilityFlags != NULL)
            {
                free(content->availabilityFlags);
                content->availabilityFlags = NULL;
            }

            free(elementsPool->content);
            elementsPool->content = NULL;
            content = NULL;
        }
    }
}
