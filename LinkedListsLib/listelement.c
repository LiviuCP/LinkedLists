#include <stdio.h>
#include <stdint.h>

#include "listelement.h"
#include "bitoperations.h"
#include "testobjects.h"
#include "error.h"

#define ELEMENTS_POOL_SLICES_COUNT 8
#define SLICE_OFFSET 1

/* The list elements pool consists of a fixed number of slices, each slice having a fixed number of elements that can be aquired for usage
   - when the pool gets created only the first slice is available
   - when the slice is fully allocated a new slice is added; only one slice can be added at a time
   - this continues until all slices have been created
   - when the last slice got filled in, no more elements can be aquired until at least one element gets released
   - the maximum number of elements that can be aquired in the same time cannot exceed the elements count of two slices
   - currently slices can only be added (allocated) but not removed/freed (i.e. when the number of aquired elements is only a small fraction of the total available ones)
   - currently the number of slices are hardcoded
*/

typedef struct
{
    uint16_t sliceIndex;
    uint16_t sliceElementIndex;
} SliceElementId;

typedef struct
{
    ListElement* elements;
    byte_t* availabilityFlags;
    size_t totalElementsCount;
    size_t availableElementsCount;
    void* data; // heap data allocated to slice, used only for allocation/deallocation purposes
} ListElementsSlice;

typedef struct
{
    ListElementsSlice** elementSlices;
    SliceElementId* sliceElementIds;
    size_t totalCount;
    size_t availableCount;
} ListElementsPoolContent;

// "private" (supporting) functions
static bool initListElementsPool(ListElementsPool* elementsPool);
static void clearListElementsPool(ListElementsPool* elementsPool);
static bool addSliceToElementsPool(ListElementsPool* elementsPool);
static bool retrieveSliceIndex(const ListElement* element, const ListElementsPool* elementsPool, size_t* sliceIndex);
static bool isValidListElementsPool(const ListElementsPool* elementsPool);
static ListElementsSlice* createSlice(size_t elementsCount);
static void deleteSlice(ListElementsSlice* slice);
static bool isValidSlice(const ListElementsSlice* slice);
static bool areAllSlicesValid(const ListElementsPool* elementsPool);
static bool isValidSliceElementId(const SliceElementId sliceElementId, const ListElementsPool* elementsPool);

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
        canElementBeAquired = (content->availableCount > 0) || addSliceToElementsPool(elementsPool);
    }

    if (canElementBeAquired)
    {
        ListElementsSlice** elementSlices = content->elementSlices;
        SliceElementId* sliceElementIds = content->sliceElementIds;

        ASSERT(content->availableCount > 0, "There should be at least one element available for aquiring!");
        const size_t lastAvailableSliceElementIdIndex = content->availableCount - 1;

        const SliceElementId lastAvailableSliceElementId = sliceElementIds[lastAvailableSliceElementIdIndex];
        const bool isValidId = isValidSliceElementId(lastAvailableSliceElementId, elementsPool);
        ASSERT(isValidId, "Invalid slice element id!");

        ListElementsSlice* slice = isValidId ? elementSlices[lastAvailableSliceElementId.sliceIndex] : NULL;
        const bool isSliceValid = isValidSlice(slice);
        ASSERT(isSliceValid, "Invalid slice!");

        ListElement* lastAvailableSliceElement = isSliceValid ? &slice->elements[lastAvailableSliceElementId.sliceElementIndex] : NULL;
        ASSERT(lastAvailableSliceElement != NULL, "NULL element to aquire detected!");

        if (lastAvailableSliceElement != NULL)
        {
            aquiredElement = lastAvailableSliceElement;
            const size_t byteIndex = lastAvailableSliceElementId.sliceElementIndex / BYTE_SIZE;
            const size_t bitIndex = BYTE_SIZE - 1 - lastAvailableSliceElementId.sliceElementIndex % BYTE_SIZE; // bits are numbered from byte end (least significant: 0) to beginning (most significant: 7)
            const byte_t elementBitMask = LSB_MASK << bitIndex;
            slice->availabilityFlags[byteIndex] &= ~elementBitMask; // bit of the element set to 0, element is aquired (hence unavailable)
            --slice->availableElementsCount;
            --content->availableCount;
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

    if (elementsPool != NULL && elements != NULL && requiredElementsCount > 0)
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
            canElementsBeAquired = (content->totalCount < ELEMENTS_POOL_SLICES_COUNT * ELEMENTS_POOL_SLICE_SIZE) && addSliceToElementsPool(elementsPool);
        }
    }

    if (canElementsBeAquired)
    {
        ListElementsSlice** elementSlices = content->elementSlices;
        SliceElementId* sliceElementIds = content->sliceElementIds;
        ASSERT(content->availableCount >= requiredElementsCount, "There should be enough elements available for aquiring!");
        size_t lastAvailableSliceElementIdIndex = content->availableCount - 1;
        bool invalidSliceElementIdDetected = false;

        for (size_t aquiredElementIndex = 0; aquiredElementIndex < requiredElementsCount; ++aquiredElementIndex)
        {
            if (!isValidSliceElementId(sliceElementIds[lastAvailableSliceElementIdIndex - aquiredElementIndex], elementsPool))
            {
                invalidSliceElementIdDetected = true;
                break;
            }
        }

        ASSERT(!invalidSliceElementIdDetected, "Invalid slice element id!");

        const bool invalidSliceDetected = !areAllSlicesValid(elementsPool);
        ASSERT(!invalidSliceDetected, "Invalid slice!");

        const size_t aquiredElementsCount = !invalidSliceElementIdDetected && !invalidSliceDetected ? requiredElementsCount : 0;

        for (size_t aquiredElementIndex = 0; aquiredElementIndex < aquiredElementsCount; ++aquiredElementIndex)
        {
            const SliceElementId lastAvailableSliceElementId = sliceElementIds[lastAvailableSliceElementIdIndex];
            ListElementsSlice* slice = elementSlices[lastAvailableSliceElementId.sliceIndex];
            ListElement* lastAvailableSliceElement = &slice->elements[lastAvailableSliceElementId.sliceElementIndex];
            const size_t byteIndex = lastAvailableSliceElementId.sliceElementIndex / BYTE_SIZE;
            const size_t bitIndex = BYTE_SIZE - 1 - lastAvailableSliceElementId.sliceElementIndex % BYTE_SIZE; // bits are numbered from byte end (least significant: 0) to beginning (most significant: 7)
            const byte_t elementBitMask = LSB_MASK << bitIndex;
            slice->availabilityFlags[byteIndex] &= ~elementBitMask; // bit of the element set to 0, element is aquired (hence unavailable)
            elements[aquiredElementIndex] = lastAvailableSliceElement;
            --slice->availableElementsCount;
            --lastAvailableSliceElementIdIndex;
        }

        content->availableCount -= aquiredElementsCount;
        success = aquiredElementsCount > 0;
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

    ListElementsSlice* slice = isElementAddressValid ? content->elementSlices[sliceIndex] : NULL;

    if (isValidSlice(slice))
    {
        const ListElement* firstElementInSlice = slice->elements;
        const size_t sliceElementIndex = element - firstElementInSlice;
        const size_t byteIndex = sliceElementIndex / BYTE_SIZE;
        const size_t bitIndex = BYTE_SIZE - 1 - sliceElementIndex % BYTE_SIZE; // bits are numbered from byte end (least significant: 0) to beginning (most significant: 7)
        const byte_t elementBitMask = LSB_MASK << bitIndex;
        const bool canElementBeAquired = slice->availabilityFlags[byteIndex] & elementBitMask; // test element availability bit

        // only already aquired elements can be released
        if (!canElementBeAquired)
        {
            const size_t releasedSliceElementIdIndex = content->availableCount;
            ASSERT(releasedSliceElementIdIndex < content->totalCount, "Count of available elements should be lower than total!");
            initListElement(element); // re-initialization of the element (it is assumed that a cleanup of the object data has been performed prior to releasing the element)
            SliceElementId* releasedSliceElementId = &content->sliceElementIds[releasedSliceElementIdIndex];
            releasedSliceElementId->sliceIndex = sliceIndex;
            releasedSliceElementId->sliceElementIndex = sliceElementIndex;
            slice->availabilityFlags[byteIndex] |= elementBitMask; // set bit, element is available again for aquiring
            ++slice->availableElementsCount;
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
    static_assert(ELEMENTS_POOL_SLICE_SIZE > 0 && ELEMENTS_POOL_SLICE_SIZE % BYTE_SIZE == 0, "Invalid slice size!");

    bool success = false;
    ListElementsPoolContent* content = NULL;
    ListElementsSlice** elementSlices = NULL;
    SliceElementId* sliceElementIds = NULL;
    const size_t totalCount = ELEMENTS_POOL_SLICE_SIZE;

    if (elementsPool != NULL)
    {
        content = malloc(sizeof(ListElementsPoolContent));
    }

    if (content != NULL)
    {
        elementSlices = (ListElementsSlice**)malloc(ELEMENTS_POOL_SLICES_COUNT * sizeof(ListElementsSlice*));
    }

    if (elementSlices != NULL)
    {
        elementSlices[0] = createSlice(totalCount);

        if (elementSlices[0] != NULL)
        {
            for (size_t sliceIndex = 1; sliceIndex < ELEMENTS_POOL_SLICES_COUNT; ++sliceIndex)
            {
                elementSlices[sliceIndex] = NULL;
            }

            sliceElementIds = (SliceElementId*)malloc(totalCount * sizeof(SliceElementId));
        }
    }

    if (sliceElementIds != NULL)
    {
        success = true;

        for (size_t index = 0; index < totalCount; ++index)
        {
            sliceElementIds[index].sliceIndex = 0;
            sliceElementIds[index].sliceElementIndex = index;
        }

        content->elementSlices = elementSlices;
        content->sliceElementIds = sliceElementIds;
        content->totalCount = totalCount;
        content->availableCount = content->totalCount;
        elementsPool->content = content;
    }

    if (!success)
    {
        FREE(content);

        if (elementSlices != NULL)
        {
            deleteSlice(elementSlices[0]);
            elementSlices[0] = NULL;

            free(elementSlices);
            elementSlices = NULL;
        }

        FREE(sliceElementIds);
    }

    return success;
}

static void clearListElementsPool(ListElementsPool* elementsPool)
{
    ListElementsPoolContent* content = NULL;
    ListElementsSlice** elementSlices = NULL;
    SliceElementId* sliceElementIds = NULL;

    if (elementsPool != NULL)
    {
        content = (ListElementsPoolContent*)elementsPool->content;
    }

    if (content != NULL)
    {
        elementSlices = content->elementSlices;
        sliceElementIds = content->sliceElementIds;
        free(content);
        content = NULL;
        elementsPool->content = NULL;
    }

    if (elementSlices != NULL)
    {
        for (size_t sliceIndex = 0; sliceIndex < ELEMENTS_POOL_SLICES_COUNT; ++sliceIndex)
        {
            deleteSlice(elementSlices[sliceIndex]);
            elementSlices[sliceIndex] = NULL;
        }

        free(elementSlices);
        elementSlices = NULL;
    }

    FREE(sliceElementIds);
}

static bool addSliceToElementsPool(ListElementsPool* elementsPool)
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
        ListElementsPoolContent* content = (ListElementsPoolContent*)elementsPool->content;

        const size_t totalCount = content->totalCount;
        const size_t newTotalCount = totalCount + ELEMENTS_POOL_SLICE_SIZE;
        const size_t slicesCount = content->totalCount / ELEMENTS_POOL_SLICE_SIZE;

        ListElementsSlice* newSlice = NULL;
        SliceElementId* newSliceElementIds = NULL;

        if (slicesCount < ELEMENTS_POOL_SLICES_COUNT)
        {
            newSlice = createSlice(ELEMENTS_POOL_SLICE_SIZE);
        }

        if (newSlice != NULL)
        {
            newSliceElementIds = (SliceElementId*)realloc(content->sliceElementIds, newTotalCount * sizeof(SliceElementId));
        }

        if (newSliceElementIds != NULL)
        {
            const size_t newSliceIndex = slicesCount;
            size_t sliceElementIdIndex = content->availableCount;

            for (size_t sliceElementIndex = 0; sliceElementIndex < ELEMENTS_POOL_SLICE_SIZE; ++sliceElementIndex)
            {
                newSliceElementIds[sliceElementIdIndex].sliceIndex = newSliceIndex;
                newSliceElementIds[sliceElementIdIndex].sliceElementIndex = sliceElementIndex;
                ++sliceElementIdIndex;
            }

            content->elementSlices[newSliceIndex] = newSlice;
            content->sliceElementIds = newSliceElementIds;
            content->totalCount = newTotalCount;
            content->availableCount += ELEMENTS_POOL_SLICE_SIZE;
            success = true;
        }
        else
        {
            deleteSlice(newSlice);
            newSlice = NULL;
        }
    }

    return success;
}

static bool retrieveSliceIndex(const ListElement* element, const ListElementsPool* elementsPool, size_t* sliceIndex)
{
    bool isValid = false;

    if (element != NULL && isValidListElementsPool(elementsPool))
    {
        const ListElementsPoolContent* const content = (ListElementsPoolContent*)elementsPool->content;
        ListElementsSlice** const elementSlices = content->elementSlices;
        const size_t slicesCount = content->totalCount / ELEMENTS_POOL_SLICE_SIZE;

        for (size_t index = 0; index < slicesCount; ++index)
        {
            const ListElement* firstSliceElement = elementSlices[index]->elements;

            if (element >= firstSliceElement && element < firstSliceElement + ELEMENTS_POOL_SLICE_SIZE)
            {
                isValid = true;

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

// Doesn't check the validity of the slices and slice element ids in detail; assumes slices have equal number of elements (ELEMENTS_POOL_SLICE_SIZE)
static bool isValidListElementsPool(const ListElementsPool* elementsPool)
{
    static_assert(ELEMENTS_POOL_SLICE_SIZE > 0 && ELEMENTS_POOL_SLICE_SIZE % BYTE_SIZE == 0, "Invalid slice size!");

    bool isValid = false;
    ListElementsPoolContent* content = NULL;

    if (elementsPool != NULL)
    {
        content = (ListElementsPoolContent*)elementsPool->content;

        isValid = content != NULL &&
                  content->elementSlices != NULL &&
                  content->sliceElementIds != NULL &&
                  content->totalCount >= ELEMENTS_POOL_SLICE_SIZE &&
                  content->totalCount >= content->availableCount;
    }

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

    return isValid;
}

static ListElementsSlice* createSlice(size_t elementsCount)
{
    ListElementsSlice* slice = NULL;
    void* data = NULL;

    const bool isValidElementsCount = elementsCount > 0 && elementsCount % BYTE_SIZE == 0;
    ASSERT(isValidElementsCount, "Invalid elements count for requested slice (should be > 0 and divisible with byte size)");

    if (isValidElementsCount)
    {
        const size_t dataSize = SLICE_OFFSET + sizeof(ListElementsSlice) + elementsCount * sizeof(ListElement) + elementsCount / BYTE_SIZE;
        data = malloc(dataSize);
    }

    if (data != NULL)
    {
        // the offset bytes are required in order to prevent de-allocating data by deleting pointer to the first category (slice object)
        slice = (ListElementsSlice*)(data + SLICE_OFFSET);
        slice->elements = (ListElement*)(slice + 1);
        slice->availabilityFlags = (byte_t*)(slice->elements + elementsCount);
        slice->totalElementsCount = elementsCount;
        slice->availableElementsCount = elementsCount;
        slice->data = data;

        for (size_t index = 0; index < elementsCount; ++index)
        {
            initListElement(slice->elements + index);
        }

        for (size_t index = 0; index < elementsCount / BYTE_SIZE; ++index)
        {
            slice->availabilityFlags[index] = MAX_BYTE_VALUE;
        }
    }

    return slice;
}

static void deleteSlice(ListElementsSlice* slice)
{
    void* data = NULL;

    if (slice != NULL)
    {
        data = slice->data;
        slice = NULL;
    }

    FREE(data);
}

static bool isValidSlice(const ListElementsSlice* slice)
{
    return slice != NULL &&
           slice->data != NULL &&
           slice->elements != NULL &&
           slice->availabilityFlags != NULL &&
           slice->totalElementsCount > 0 &&
           slice->totalElementsCount % BYTE_SIZE == 0 &&
           slice->availableElementsCount <= slice->totalElementsCount;
}

static bool areAllSlicesValid(const ListElementsPool* elementsPool)
{
    const ListElementsPoolContent* content = elementsPool != NULL ? (ListElementsPoolContent*)elementsPool->content : NULL;
    bool areValid = content != NULL;

    if (areValid)
    {
        const size_t slicesCount = content->totalCount / ELEMENTS_POOL_SLICE_SIZE;

        for (size_t sliceIndex = 0; sliceIndex < slicesCount; ++sliceIndex)
        {
            if (!isValidSlice(content->elementSlices[sliceIndex]))
            {
                areValid = false;
                break;
            }
        }
    }

    return areValid;
}

static bool isValidSliceElementId(const SliceElementId sliceElementId, const ListElementsPool* elementsPool)
{
    bool isValid = false;
    const ListElementsPoolContent* content = NULL;

    if (elementsPool != NULL)
    {
        content = (ListElementsPoolContent*)elementsPool->content;
        ASSERT(content != NULL, "NULL list elements pool content!");
    }

    if (content != NULL)
    {
        const ListElementsSlice* slice = NULL;
        const size_t slicesCount = content->totalCount / ELEMENTS_POOL_SLICE_SIZE;

        ASSERT(content->elementSlices != NULL, "NULL element slices!");

        if (sliceElementId.sliceIndex < slicesCount && content->elementSlices != NULL)
        {
            slice = content->elementSlices[sliceElementId.sliceIndex];
            ASSERT(slice != NULL, "NULL list elements pool slice!");

            isValid = slice != NULL && sliceElementId.sliceElementIndex < slice->totalElementsCount;
        }
    }

    return isValid;
}
