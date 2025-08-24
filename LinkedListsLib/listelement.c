#include <stdio.h>

#include "listelement.h"
#include "testobjects.h"
#include "error.h"

typedef struct
{
    ListElement* data;
    ListElement** dataRefs;
    size_t itemsCount;
    size_t availableItemsCount;
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
            count = content->availableItemsCount;
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
        ASSERT(content != NULL, "Invalid list elements elementsPool content!");

        if (content != NULL && content->availableItemsCount > 0)
        {
            ListElement** dataRefs = content->dataRefs;
            ASSERT(dataRefs != NULL, "Invalid list elements elementsPool data refs!")

            if (dataRefs != NULL)
            {
                const size_t aquiredElementIndex = content->availableItemsCount - 1;

                aquiredElement = dataRefs[aquiredElementIndex];
                dataRefs[aquiredElementIndex] = NULL;
                --content->availableItemsCount;
            }
        }
    }

    return aquiredElement;
}

// TODO: implement double release prevention mechanism if possible
bool releaseElement(ListElement* element, ListElementsPool* elementsPool)
{
    bool success = false;

    if (elementsPool != NULL && element != NULL)
    {
        ListElementsPoolContent* content = (ListElementsPoolContent*)elementsPool->content;

        const bool isValidPool = content != NULL && content->data != NULL && content->dataRefs != NULL && content->itemsCount > 0 && content->itemsCount > content->availableItemsCount;
        ASSERT(isValidPool, "Invalid list elements elementsPool parameters!")

        if (isValidPool && element >= content->data && element < &content->data[content->itemsCount])
        {
            element->next = NULL;
            element->object.type = -1;
            element->object.payload = NULL;
            element->priority = 0;
            content->dataRefs[content->availableItemsCount] = element;
            ++content->availableItemsCount;
            success = true;
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
    ListElement* data = NULL;
    ListElement** dataRefs = NULL;

    if (elementsPool != NULL)
    {
        content = malloc(sizeof(ListElementsPoolContent));
    }

    if (content != NULL)
    {
        data = (ListElement*)malloc(MAX_POOL_ITEMS_COUNT * sizeof(ListElement));
    }

    if (data != NULL)
    {
        dataRefs = (ListElement**)malloc(MAX_POOL_ITEMS_COUNT * sizeof(ListElement*));
    }

    if (dataRefs != NULL)
    {
        success = true;

        for (size_t index = 0; index < MAX_POOL_ITEMS_COUNT; ++index)
        {
            data[index].next = NULL;
            data[index].object.type = -1;
            data[index].object.payload = NULL;
            data[index].priority = 0;
            dataRefs[index] = &data[index];
        }

        content->data = data;
        content->dataRefs = dataRefs;
        content->itemsCount = MAX_POOL_ITEMS_COUNT;
        content->availableItemsCount = MAX_POOL_ITEMS_COUNT;
        elementsPool->content = content;
    }

    if (!success)
    {
        if (content != NULL)
        {
            free(content);
            content = NULL;
        }

        if (data != NULL)
        {
            free(data);
            data = NULL;
        }

        if (dataRefs != NULL)
        {
            free(dataRefs);
            dataRefs = NULL;
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
            if (content->data != NULL)
            {
                free(content->data);
                content->data = NULL;
            }

            if (content->dataRefs != NULL)
            {
                free(content->dataRefs);
                content->dataRefs = NULL;
            }

            free(elementsPool->content);
            elementsPool->content = NULL;
            content = NULL;
        }
    }
}
