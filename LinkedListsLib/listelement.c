#include <stdio.h>

#include "listelement.h"
#include "bitoperations.h"
#include "testobjects.h"
#include "error.h"

#define ELEMENTS_POOL_SLICES_COUNT 8

/* The list elements pool consists of a fixed number of slices, each slice having a fixed number of elements that can be aquired for usage
   - when the pool gets created only the first slice is available
   - when the slice is fully allocated a new slice is added; only one slice can be added at a time
   - this continues until all slices have been created
   - when the last slice got filled in, no more elements can be aquired until at least one element gets released
   - the maximum number of elements that can be aquired in the same time cannot exceed the elements count of two slices
   - currently slices can only be added (allocated) but not removed/freed (i.e. when the number of aquired elements is only a small fraction of the total available ones)
   - currently the number of slices and the elements count per slice are hardcoded
*/

typedef struct
{
    ListElement** elementSlices;
    ListElement** elementRefs;
    byte_t* availabilityFlags;
    size_t totalCount;
    size_t availableCount;
} ListElementsPoolContent;

// "private" (supporting) functions
static bool initListElementsPool(ListElementsPool* elementsPool);
static void clearListElementsPool(ListElementsPool* elementsPool);
static bool addSlice(ListElementsPool* elementsPool);
static bool isValidListElementsPool(const ListElementsPool* elementsPool);
static bool retrieveSliceIndex(const ListElement* element, const ListElementsPool* elementsPool, size_t* sliceIndex);

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
        clearListElementsPool(elementsPool);
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

ListElement* aquireElement(ListElementsPool* elementsPool)
{
    ListElement* aquiredElement = NULL;

    bool isValidElementsPool = false;
    ListElementsPoolContent* content = NULL;
    bool canElementBeAquired = false;

    if (elementsPool != NULL)
    {
        isValidElementsPool = isValidListElementsPool(elementsPool);
        ASSERT(isValidElementsPool, "Invalid list elements pool parameters!");
    }

    if (isValidElementsPool)
    {
        content = (ListElementsPoolContent*)elementsPool->content;
        canElementBeAquired = content->availableCount > 0;

        if (!canElementBeAquired)
        {
            canElementBeAquired = addSlice(elementsPool);
        }
    }

    if (canElementBeAquired)
    {
        ListElement** elementSlices = content->elementSlices;
        ListElement** elementRefs = content->elementRefs;
        byte_t* availabilityFlags = content->availabilityFlags;

        const size_t lastAvailableElementRefIndex = content->availableCount - 1;
        ListElement* const elementToAquire = elementRefs[lastAvailableElementRefIndex];

        ASSERT(elementToAquire != NULL, "NULL element to aquire detected!");

        if (elementToAquire != NULL)
        {
            size_t sliceIndex = 0;
            const bool isElementAddressValid = retrieveSliceIndex(elementToAquire, elementsPool, &sliceIndex);
            ASSERT(isElementAddressValid, "Invalid element address detected!");

            if (isElementAddressValid)
            {
                aquiredElement = elementRefs[lastAvailableElementRefIndex];
                const ListElement* firstElementInSlice = elementSlices[sliceIndex];
                const size_t elementIndex = sliceIndex * ELEMENTS_POOL_SLICE_SIZE + aquiredElement - firstElementInSlice;
                const size_t byteIndex = elementIndex / BYTE_SIZE;
                const size_t bitIndex = BYTE_SIZE - 1 - elementIndex % BYTE_SIZE; // bits are numbered from byte end (least significant: 0) to beginning (most significant: 7)
                const byte_t elementBitMask = LSB_MASK << bitIndex;
                availabilityFlags[byteIndex] &= ~elementBitMask; // bit of the element set to 0, element is aquired (hence unavailable)
                elementRefs[lastAvailableElementRefIndex] = NULL;
                --content->availableCount;
            }
        }
    }

    return aquiredElement;
}

bool aquireElements(ListElementsPool* elementsPool, ListElement** elements, size_t requiredElementsCount)
{
    bool success = false;

    bool isValidElementsPool = false;
    ListElementsPoolContent* content = NULL;
    bool canElementsBeAquired = false;

    if (elementsPool != NULL && elements != NULL && requiredElementsCount > 0 && requiredElementsCount > 0)
    {
        isValidElementsPool = isValidListElementsPool(elementsPool);
        ASSERT(isValidElementsPool, "Invalid list elements pool parameters!");
    }

    // the maximum number of elements to be aquired cannot exceed 2 slices, i.e. maximum one slice can be created per aquiring operation provided the maximum slices count hasn't been reached
    if (isValidElementsPool)
    {
        content = (ListElementsPoolContent*)elementsPool->content;
        const size_t maxAllowedCountToBeAquired = ((content->availableCount < ELEMENTS_POOL_SLICE_SIZE ? content->availableCount : ELEMENTS_POOL_SLICE_SIZE) + ELEMENTS_POOL_SLICE_SIZE);
        canElementsBeAquired = requiredElementsCount <= maxAllowedCountToBeAquired;

        if (canElementsBeAquired && requiredElementsCount > content->availableCount)
        {
            canElementsBeAquired = content->totalCount < ELEMENTS_POOL_SLICES_COUNT * ELEMENTS_POOL_SLICE_SIZE;

            if (canElementsBeAquired)
            {
                canElementsBeAquired = addSlice(elementsPool);
            }
        }
    }

    if (canElementsBeAquired)
    {
        ListElement** elementSlices = content->elementSlices;
        ListElement** elementRefs = content->elementRefs;
        byte_t* availabilityFlags = content->availabilityFlags;
        size_t lastAvailableElementRefIndex = content->availableCount - 1;
        bool isElementAddressInvalid = false;

        for (size_t elementToAquireIndex = 0; elementToAquireIndex < requiredElementsCount; ++elementToAquireIndex)
        {
            ListElement* elementToAquire = elementRefs[lastAvailableElementRefIndex - elementToAquireIndex];
            isElementAddressInvalid = !retrieveSliceIndex(elementToAquire, elementsPool, NULL);

            if (isElementAddressInvalid)
            {
                break;
            }
        }

        ASSERT(!isElementAddressInvalid, "Invalid element address detected!");

        if (!isElementAddressInvalid)
        {
            for (size_t aquiredElementIndex = 0; aquiredElementIndex < requiredElementsCount; ++aquiredElementIndex)
            {
                ListElement* aquiredElement = elementRefs[lastAvailableElementRefIndex];
                size_t sliceIndex = 0;
                (void)retrieveSliceIndex(aquiredElement, elementsPool, &sliceIndex); // return value already checked in previous loop
                const ListElement* firstElementInSlice = elementSlices[sliceIndex];
                const size_t elementIndex = sliceIndex * ELEMENTS_POOL_SLICE_SIZE + aquiredElement - firstElementInSlice;
                const size_t byteIndex = elementIndex / BYTE_SIZE;
                const size_t bitIndex = BYTE_SIZE - 1 - elementIndex % BYTE_SIZE; // bits are numbered from byte end (least significant: 0) to beginning (most significant: 7)
                const byte_t elementBitMask = LSB_MASK << bitIndex;

                availabilityFlags[byteIndex] &= ~elementBitMask; // bit of the element set to 0, element is aquired (hence unavailable)
                elements[aquiredElementIndex] = aquiredElement;
                elementRefs[lastAvailableElementRefIndex] = NULL;
                --lastAvailableElementRefIndex;
            }

            content->availableCount -= requiredElementsCount;
            success = true;
        }
    }

    return success;
}

bool releaseElement(ListElement* element, ListElementsPool* elementsPool)
{
    bool success = false;

    bool isValidElementsPool = false;
    ListElementsPoolContent* content;
    bool isElementAddressValid = false;
    size_t sliceIndex = 0;

    if (elementsPool != NULL && element != NULL)
    {
        isValidElementsPool = isValidListElementsPool(elementsPool);
        ASSERT(isValidElementsPool, "Invalid list elements elementsPool parameters!");
    }

    if (isValidElementsPool)
    {
        content = (ListElementsPoolContent*)elementsPool->content;
        isElementAddressValid = retrieveSliceIndex(element, elementsPool, &sliceIndex);
    }

    if (isElementAddressValid)
    {
        const ListElement* firstElementInSlice = content->elementSlices[sliceIndex];
        const size_t elementIndex = sliceIndex * ELEMENTS_POOL_SLICE_SIZE + element - firstElementInSlice;
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
    ListElement** elementSlices = NULL;
    ListElement** elementRefs = NULL;
    byte_t* availabilityFlags = NULL;
    const size_t totalCount = ELEMENTS_POOL_SLICE_SIZE;
    const size_t availabilityBytesCount = totalCount / BYTE_SIZE + (totalCount % BYTE_SIZE > 0 ? 1 : 0);

    if (elementsPool != NULL)
    {
        content = malloc(sizeof(ListElementsPoolContent));
    }

    if (content != NULL)
    {
        elementSlices = (ListElement**)malloc(ELEMENTS_POOL_SLICES_COUNT * sizeof(ListElement*));
    }

    if (elementSlices != NULL)
    {
        elementSlices[0] = (ListElement*)malloc(totalCount * sizeof(ListElement));

        if (elementSlices[0] != NULL)
        {
            for (size_t sliceIndex = 1; sliceIndex < ELEMENTS_POOL_SLICES_COUNT; ++sliceIndex)
            {
                elementSlices[sliceIndex] = NULL;
            }

            elementRefs = (ListElement**)malloc(totalCount * sizeof(ListElement*));
        }
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
            ListElement* element = elementSlices[0] + index;
            initListElement(element);
            elementRefs[index] = element;
        }

        for (size_t index = 0; index < availabilityBytesCount; ++index)
        {
            availabilityFlags[index] = MAX_BYTE_VALUE;
        }

        content->elementSlices = elementSlices;
        content->elementRefs = elementRefs;
        content->availabilityFlags = availabilityFlags;
        content->totalCount = totalCount;
        ASSERT(content->totalCount > 0, "Total pooled elements count should not be 0!");
        content->availableCount = content->totalCount;
        elementsPool->content = content;
    }

    if (!success)
    {
        FREE(content);

        if (elementSlices != NULL)
        {
            FREE(elementSlices[0]);

            free(elementSlices);
            elementSlices = NULL;
        }

        FREE(elementRefs);
        FREE(availabilityFlags);
    }

    return success;
}

static void clearListElementsPool(ListElementsPool* elementsPool)
{
    if (elementsPool != NULL)
    {
        ListElementsPoolContent* content = (ListElementsPoolContent*)elementsPool->content;

        if (content != NULL)
        {
            if (content->elementSlices != NULL)
            {
                ListElement** elementSlices = content->elementSlices;

                for (size_t sliceIndex = 0; sliceIndex < ELEMENTS_POOL_SLICES_COUNT; ++sliceIndex)
                {
                    FREE(elementSlices[sliceIndex]);
                }

                free(content->elementSlices);
                content->elementSlices = NULL;
            }

            FREE(content->elementRefs);
            FREE(content->availabilityFlags);

            free(elementsPool->content);
            elementsPool->content = NULL;
            content = NULL;
        }
    }
}

static bool addSlice(ListElementsPool* elementsPool)
{
    bool success = false;
    bool isValidPool = false;

    if (elementsPool)
    {
        isValidPool = isValidListElementsPool(elementsPool);
        ASSERT(isValidPool, "Invalid list elements pool detected!");
    }

    if (isValidPool)
    {
        ListElementsPoolContent* content = elementsPool->content;

        const size_t totalCount = content->totalCount;
        const size_t newTotalCount = totalCount + ELEMENTS_POOL_SLICE_SIZE;
        const size_t availabilityBytesCount = totalCount / BYTE_SIZE + (totalCount % BYTE_SIZE > 0 ? 1 : 0);
        const size_t newAvailabilityBytesCount = newTotalCount / BYTE_SIZE + (newTotalCount % BYTE_SIZE > 0 ? 1 : 0);
        const size_t slicesCount = content->totalCount / ELEMENTS_POOL_SLICE_SIZE;
        const size_t newSlicesCount = slicesCount + 1;

        ListElement* newSlice = NULL;
        ListElement** newElementRefs = NULL;
        byte_t* newAvailabilityFlags = NULL;

        if (slicesCount < ELEMENTS_POOL_SLICES_COUNT)
        {
            newSlice = (ListElement*)malloc(ELEMENTS_POOL_SLICE_SIZE * sizeof(ListElement));
        }

        if (newSlice != NULL)
        {
            newElementRefs = (ListElement**)malloc((newSlicesCount) * ELEMENTS_POOL_SLICE_SIZE * sizeof(ListElement*));
        }

        if (newElementRefs != NULL)
        {
            newAvailabilityFlags = (byte_t*)malloc(newAvailabilityBytesCount);
        }

        if (newAvailabilityFlags != NULL)
        {
            size_t elementRefIndex = 0;

            for (; elementRefIndex < content->availableCount; ++elementRefIndex)
            {
                newElementRefs[elementRefIndex] = content->elementRefs[elementRefIndex];
                content->elementRefs[elementRefIndex] = NULL;
            }

            for (size_t sliceElementIndex = 0; sliceElementIndex < ELEMENTS_POOL_SLICE_SIZE; ++sliceElementIndex)
            {
                newElementRefs[elementRefIndex] = newSlice + sliceElementIndex;
                ++elementRefIndex;
            }

            size_t byteIndex = 0;

            for (; byteIndex < availabilityBytesCount; ++byteIndex)
            {
                newAvailabilityFlags[byteIndex] = content->availabilityFlags[byteIndex];
                content->availabilityFlags[byteIndex] = 0;
            }

            // there should be no bit misalignment issues (in case there are more bits than number of elements) - additional bits are initialized with 1 each time new elements are added
            // however a good practice would be to have a number of elements that is multiple of the byte size
            for (; byteIndex < newAvailabilityBytesCount; ++byteIndex)
            {
                newAvailabilityFlags[byteIndex] = MAX_BYTE_VALUE;
            }

            free(content->elementRefs);
            free(content->availabilityFlags);

            for (size_t index = 0; index < ELEMENTS_POOL_SLICE_SIZE; ++index)
            {
                ListElement* element = newSlice + index;
                initListElement(element);
            }

            content->elementSlices[slicesCount] = newSlice;
            content->elementRefs = newElementRefs;
            content->availabilityFlags = newAvailabilityFlags;
            content->totalCount = newTotalCount;
            content->availableCount += ELEMENTS_POOL_SLICE_SIZE;

            success = true;
        }
        else
        {
            FREE(newSlice);
            FREE(newElementRefs);
        }
    }

    return success;
}

static bool isValidListElementsPool(const ListElementsPool* elementsPool)
{
    static_assert(ELEMENTS_POOL_SLICE_SIZE > 0, "Invalid slice size!");

    bool isValid = false;
    ASSERT(elementsPool != NULL, "The list elements pool to be checked should NOT be NULL!");

    if (elementsPool != NULL)
    {
        ListElementsPoolContent* content = (ListElementsPoolContent*)elementsPool->content;

        isValid = content != NULL &&
                  content->elementSlices != NULL &&
                  content->elementRefs != NULL &&
                  content->availabilityFlags != NULL &&
                  content->totalCount >= ELEMENTS_POOL_SLICE_SIZE &&
                  content->totalCount >= content->availableCount;

        if (isValid)
        {
            const size_t slicesCount = content->totalCount / ELEMENTS_POOL_SLICE_SIZE;

            for (size_t sliceIndex = 0; sliceIndex < slicesCount; ++sliceIndex)
            {
                if (content->elementSlices[sliceIndex] == NULL)
                {
                    isValid = false;
                    break;
                }
            }
        }
    }

    return isValid;
}

static bool retrieveSliceIndex(const ListElement* element, const ListElementsPool* elementsPool, size_t* sliceIndex)
{
    bool isValid = false;

    if (element != NULL && isValidListElementsPool(elementsPool))
    {
        const ListElementsPoolContent* const content = (ListElementsPoolContent*)elementsPool->content;
        ListElement** const elementSlices = content->elementSlices;
        const size_t slicesCount = content->totalCount / ELEMENTS_POOL_SLICE_SIZE;

        for (size_t index = 0; index < slicesCount; ++index)
        {
            isValid = element >= elementSlices[index] && element < elementSlices[index] + ELEMENTS_POOL_SLICE_SIZE;

            if (isValid)
            {
                if (sliceIndex != NULL)
                {
                    *sliceIndex = index;
                }

                break;
            }
        }
    }

    return isValid;
}
