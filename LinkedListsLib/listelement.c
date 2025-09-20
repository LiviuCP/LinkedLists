#include <stdio.h>
#include <stdint.h>

#include "listelement.h"
#include "bitoperations.h"
#include "testobjects.h"
#include "error.h"

#define ELEMENTS_POOL_MAX_SLICES_COUNT 8
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
    size_t totalElementsCount;
    size_t availableElementsCount;
    size_t slicesCount;
    size_t maximumSlicesCount;
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
        ListElementsPoolContent* poolContent = (ListElementsPoolContent*)elementsPool->poolContent;
        ASSERT(poolContent != NULL, "Invalid list elements elementsPool poolContent!");

        if (poolContent != NULL)
        {
            count = poolContent->availableElementsCount;
        }
    }

    return count;
}

ListElement* aquireElement(ListElementsPool* elementsPool)
{
    ListElement* aquiredElement = NULL;
    ListElementsPoolContent* poolContent = NULL;
    ListElementsSlice** elementSlices = NULL;
    SliceElementId* sliceElementIds = NULL;

    if (elementsPool != NULL)
    {
        poolContent = (ListElementsPoolContent*)elementsPool->poolContent;
        ASSERT(poolContent, "NULL elements pool content!");
    }

    const bool canElementBeAquired = poolContent != NULL ? (poolContent->availableElementsCount > 0) || addSliceToElementsPool(elementsPool) : false;

    if (canElementBeAquired)
    {
        elementSlices = poolContent->elementSlices;
        sliceElementIds = poolContent->sliceElementIds;
        ASSERT(elementSlices != NULL, "NULL element slices!");
        ASSERT(sliceElementIds != NULL, "NULL slice element Ids!");
    }

    if (elementSlices != NULL && sliceElementIds != NULL)
    {
        ListElementsSlice* slice = NULL;
        ListElement* lastAvailableSliceElement = NULL;

        ASSERT(poolContent->availableElementsCount > 0, "There should be at least one element available for aquiring!");

        if (poolContent->availableElementsCount > 0)
        {
            const size_t lastAvailableSliceElementIdIndex = poolContent->availableElementsCount - 1;
            const SliceElementId lastAvailableSliceElementId = sliceElementIds[lastAvailableSliceElementIdIndex];
            const bool isValidIdSliceIndex = lastAvailableSliceElementId.sliceIndex < poolContent->slicesCount;

            slice = isValidIdSliceIndex ? elementSlices[lastAvailableSliceElementId.sliceIndex] : NULL;

            const bool isSliceValid = slice != NULL && slice->elements != NULL;
            const bool isSliceElementIndexValid = isSliceValid && lastAvailableSliceElementId.sliceElementIndex < slice->totalElementsCount;

            lastAvailableSliceElement = isSliceElementIndexValid ? &slice->elements[lastAvailableSliceElementId.sliceElementIndex] : NULL;
            ASSERT(lastAvailableSliceElement != NULL, "NULL element to aquire detected!");

            if (lastAvailableSliceElement != NULL)
            {
                aquiredElement = lastAvailableSliceElement;
                const size_t byteIndex = lastAvailableSliceElementId.sliceElementIndex / BYTE_SIZE;
                const size_t bitIndex = BYTE_SIZE - 1 - lastAvailableSliceElementId.sliceElementIndex % BYTE_SIZE; // bits are numbered from byte end (least significant: 0) to beginning (most significant: 7)
                const byte_t elementBitMask = LSB_MASK << bitIndex;
                slice->availabilityFlags[byteIndex] &= ~elementBitMask; // bit of the element set to 0, element is aquired (hence unavailable)
                --slice->availableElementsCount;
                --poolContent->availableElementsCount;
            }
        }
    }

    return aquiredElement;
}

bool aquireElements(ListElementsPool* elementsPool, ListElement** elements, size_t requiredElementsCount)
{
    bool success = false;
    ListElementsPoolContent* poolContent = NULL;
    ListElementsSlice** elementSlices = NULL;
    SliceElementId* sliceElementIds = NULL;
    bool canElementsBeAquired = false;

    if (elementsPool != NULL && elements != NULL && requiredElementsCount > 0)
    {
        poolContent = (ListElementsPoolContent*)elementsPool->poolContent;
        ASSERT(poolContent, "NULL pool content!");
    }

    // the maximum number of elements to be aquired cannot exceed 2 slices, i.e. maximum one slice can be created per aquiring operation provided the maximum slices count hasn't been reached
    if (poolContent != NULL)
    {
        const size_t maxAllowedCountToBeAquired = ((poolContent->availableElementsCount < ELEMENTS_POOL_SLICE_SIZE ? poolContent->availableElementsCount : ELEMENTS_POOL_SLICE_SIZE) + ELEMENTS_POOL_SLICE_SIZE);
        canElementsBeAquired = requiredElementsCount <= maxAllowedCountToBeAquired;

        if (canElementsBeAquired && requiredElementsCount > poolContent->availableElementsCount)
        {
            canElementsBeAquired = (poolContent->slicesCount < poolContent->maximumSlicesCount) && addSliceToElementsPool(elementsPool);
        }
    }

    if (canElementsBeAquired)
    {
        elementSlices = poolContent->elementSlices;
        sliceElementIds = poolContent->sliceElementIds;
        ASSERT(elementSlices != NULL, "NULL element slices!");
        ASSERT(sliceElementIds != NULL, "NULL slice element ids!");
    }

    bool areEnoughAvailableElements = false;

    if (elementSlices != NULL && sliceElementIds != NULL)
    {
        areEnoughAvailableElements = poolContent->availableElementsCount >= requiredElementsCount;
        ASSERT(areEnoughAvailableElements, "There should be enough elements available for aquiring!");
    }

    if (areEnoughAvailableElements)
    {
        size_t aquiredElementsCount = 0;

        for (size_t aquiredElementIndex = 0; aquiredElementIndex < requiredElementsCount; ++aquiredElementIndex)
        {
            const size_t lastAvailableSliceElementIdIndex = poolContent->availableElementsCount - aquiredElementIndex - 1;
            const SliceElementId lastAvailableSliceElementId = sliceElementIds[lastAvailableSliceElementIdIndex];

            if (lastAvailableSliceElementId.sliceIndex >= poolContent->slicesCount)
            {
                ASSERT(false, "Invalid slice index");
                break;
            }

            ListElementsSlice* slice = elementSlices[lastAvailableSliceElementId.sliceIndex];

            if (slice == NULL || slice->elements == NULL || slice->availabilityFlags == NULL || lastAvailableSliceElementId.sliceElementIndex >= slice->totalElementsCount)
            {
                ASSERT(false, "");
                break;
            }

            ListElement* lastAvailableSliceElement = &slice->elements[lastAvailableSliceElementId.sliceElementIndex];

            if (lastAvailableSliceElement == NULL)
            {
                ASSERT(false, "NULL slice element address!");
                break;
            }

            const size_t byteIndex = lastAvailableSliceElementId.sliceElementIndex / BYTE_SIZE;
            const size_t bitIndex = BYTE_SIZE - 1 - lastAvailableSliceElementId.sliceElementIndex % BYTE_SIZE; // bits are numbered from byte end (least significant: 0) to beginning (most significant: 7)
            const byte_t elementBitMask = LSB_MASK << bitIndex;
            slice->availabilityFlags[byteIndex] &= ~elementBitMask; // bit of the element set to 0, element is aquired (hence unavailable)
            elements[aquiredElementIndex] = lastAvailableSliceElement;
            --slice->availableElementsCount;
            ++aquiredElementsCount;
        }

        poolContent->availableElementsCount -= aquiredElementsCount;
        success = aquiredElementsCount == requiredElementsCount;
    }

    return success;
}

bool releaseElement(ListElement* element, ListElementsPool* elementsPool)
{
    bool success = false;
    ListElementsPoolContent* poolContent = NULL;
    ListElementsSlice** elementSlices = NULL;
    SliceElementId* sliceElementIds = NULL;
    size_t sliceIndex = 0;

    if (elementsPool != NULL && element != NULL)
    {
        poolContent = (ListElementsPoolContent*)elementsPool->poolContent;
        ASSERT(poolContent != NULL, "NULL pool content!");
    }

    if (poolContent != NULL)
    {
        elementSlices = poolContent->elementSlices;
        sliceElementIds = poolContent->sliceElementIds;
        ASSERT(elementSlices != NULL, "NULL element slices!");
        ASSERT(sliceElementIds != NULL, "NULL slice element Ids!");
    }

    const bool isElementAddressValid = elementSlices != NULL && sliceElementIds != NULL ? retrieveSliceIndex(element, elementsPool, &sliceIndex) : false;
    ListElementsSlice* slice = isElementAddressValid ? elementSlices[sliceIndex] : NULL;

    ListElement* sliceElements = NULL;
    byte_t* availabilityFlags = NULL;

    if (slice != NULL)
    {
        sliceElements = slice->elements;
        availabilityFlags = slice->availabilityFlags;
        ASSERT(sliceElements != NULL, "NULL slice elements!");
        ASSERT(availabilityFlags != NULL, "NULL availability flags!");
    }

    if (sliceElements != NULL && availabilityFlags != NULL)
    {
        const ListElement* firstElementInSlice = sliceElements;
        const size_t sliceElementIndex = element - firstElementInSlice;
        const size_t byteIndex = sliceElementIndex / BYTE_SIZE;
        const size_t bitIndex = BYTE_SIZE - 1 - sliceElementIndex % BYTE_SIZE; // bits are numbered from byte end (least significant: 0) to beginning (most significant: 7)
        const byte_t elementBitMask = LSB_MASK << bitIndex;
        const bool canElementBeAquired = availabilityFlags[byteIndex] & elementBitMask; // test element availability bit

        // only already aquired elements can be released
        if (!canElementBeAquired)
        {
            const size_t releasedSliceElementIdIndex = poolContent->availableElementsCount;
            ASSERT(releasedSliceElementIdIndex < poolContent->totalElementsCount, "Count of available elements should be lower than total!");
            initListElement(element); // re-initialization of the element (it is assumed that a cleanup of the object data has been performed prior to releasing the element)
            SliceElementId* releasedSliceElementId = &poolContent->sliceElementIds[releasedSliceElementIdIndex];
            ASSERT(releasedSliceElementId != NULL, "NULL slice element ID!");

            if (releasedSliceElementId != NULL)
            {
                releasedSliceElementId->sliceIndex = sliceIndex;
                releasedSliceElementId->sliceElementIndex = sliceElementIndex;
                availabilityFlags[byteIndex] |= elementBitMask; // set bit, element is available again for aquiring
                ++slice->availableElementsCount;
                ++poolContent->availableElementsCount;
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
    static_assert(ELEMENTS_POOL_SLICE_SIZE > 0 && ELEMENTS_POOL_SLICE_SIZE % BYTE_SIZE == 0, "Invalid slice size!");
    static_assert(ELEMENTS_POOL_MAX_SLICES_COUNT >= 1, "The list elements pool should have at least one slice!");

    bool success = false;
    ListElementsPoolContent* poolContent = NULL;
    ListElementsSlice** elementSlices = NULL;
    SliceElementId* sliceElementIds = NULL;
    const size_t totalElementsCount = ELEMENTS_POOL_SLICE_SIZE;
    const size_t slicesCount = 1;
    const size_t maximumSlicesCount = ELEMENTS_POOL_MAX_SLICES_COUNT;

    if (elementsPool != NULL)
    {
        poolContent = malloc(sizeof(ListElementsPoolContent));
    }

    if (poolContent != NULL)
    {
        elementSlices = (ListElementsSlice**)malloc(maximumSlicesCount * sizeof(ListElementsSlice*));
    }

    if (elementSlices != NULL)
    {
        elementSlices[0] = createSlice(totalElementsCount);

        if (elementSlices[0] != NULL)
        {
            for (size_t sliceIndex = 1; sliceIndex < maximumSlicesCount; ++sliceIndex)
            {
                elementSlices[sliceIndex] = NULL;
            }

            sliceElementIds = (SliceElementId*)malloc(totalElementsCount * sizeof(SliceElementId));
        }
    }

    if (sliceElementIds != NULL)
    {
        success = true;

        for (size_t index = 0; index < totalElementsCount; ++index)
        {
            sliceElementIds[index].sliceIndex = 0;
            sliceElementIds[index].sliceElementIndex = index;
        }

        poolContent->elementSlices = elementSlices;
        poolContent->sliceElementIds = sliceElementIds;
        poolContent->totalElementsCount = totalElementsCount;
        poolContent->availableElementsCount = poolContent->totalElementsCount;
        poolContent->slicesCount = 1;
        poolContent->maximumSlicesCount = maximumSlicesCount;
        elementsPool->poolContent = poolContent;
    }

    if (!success)
    {
        FREE(poolContent);

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
    ListElementsPoolContent* poolContent = NULL;
    ListElementsSlice** elementSlices = NULL;
    SliceElementId* sliceElementIds = NULL;
    size_t maximumSlicesCount = 0;

    if (elementsPool != NULL)
    {
        poolContent = (ListElementsPoolContent*)elementsPool->poolContent;
    }

    if (poolContent != NULL)
    {
        maximumSlicesCount = poolContent->maximumSlicesCount;
        elementSlices = poolContent->elementSlices;
        sliceElementIds = poolContent->sliceElementIds;
        free(poolContent);
        poolContent = NULL;
        elementsPool->poolContent = NULL;
    }

    if (elementSlices != NULL)
    {
        for (size_t sliceIndex = 0; sliceIndex < maximumSlicesCount; ++sliceIndex)
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
    ListElementsPoolContent* poolContent = NULL;
    ListElementsSlice** elementSlices = NULL;
    SliceElementId* sliceElementIds = NULL;
    ListElementsSlice* newSlice = NULL;
    SliceElementId* newSliceElementIds = NULL;
    size_t newTotalElementsCount = 0;

    if (elementsPool != NULL)
    {
        poolContent = (ListElementsPoolContent*)elementsPool->poolContent;
        ASSERT(poolContent != NULL, "NULL pool content!");
    }

    if (poolContent != NULL)
    {
        elementSlices = poolContent->elementSlices;
        sliceElementIds = poolContent->sliceElementIds;
        newTotalElementsCount = poolContent->totalElementsCount;
        ASSERT(elementSlices != NULL, "NULL element slices!");
        ASSERT(sliceElementIds != NULL, "NULL slice element IDs");
    }

    if (elementSlices != NULL && sliceElementIds != NULL && poolContent->slicesCount < poolContent->maximumSlicesCount)
    {
        newSlice = createSlice(ELEMENTS_POOL_SLICE_SIZE);
    }

    if (newSlice != NULL)
    {
        newTotalElementsCount += newSlice->totalElementsCount;
        newSliceElementIds = (SliceElementId*)realloc(sliceElementIds, newTotalElementsCount * sizeof(SliceElementId));
    }

    if (newSliceElementIds != NULL)
    {
        const size_t newSliceIndex = poolContent->slicesCount;
        size_t sliceElementIdIndex = poolContent->availableElementsCount;

        for (size_t sliceElementIndex = 0; sliceElementIndex < newSlice->totalElementsCount; ++sliceElementIndex)
        {
            newSliceElementIds[sliceElementIdIndex].sliceIndex = newSliceIndex;
            newSliceElementIds[sliceElementIdIndex].sliceElementIndex = sliceElementIndex;
            ++sliceElementIdIndex;
        }

        poolContent->elementSlices[newSliceIndex] = newSlice;
        poolContent->sliceElementIds = newSliceElementIds;
        poolContent->totalElementsCount = newTotalElementsCount;
        poolContent->availableElementsCount += newSlice->totalElementsCount;
        ++poolContent->slicesCount;
        success = true;
    }
    else
    {
        deleteSlice(newSlice);
        newSlice = NULL;
    }

    return success;
}

static bool retrieveSliceIndex(const ListElement* element, const ListElementsPool* elementsPool, size_t* sliceIndex)
{
    bool isValid = false;

    if (element != NULL && isValidListElementsPool(elementsPool))
    {
        const ListElementsPoolContent* const poolContent = (ListElementsPoolContent*)elementsPool->poolContent;
        ListElementsSlice** const elementSlices = poolContent->elementSlices;

        for (size_t index = 0; index < poolContent->slicesCount; ++index)
        {
            const ListElement* firstSliceElement = elementSlices[index]->elements;
            const size_t totalSliceElementsCount = elementSlices[index]->totalElementsCount;

            if (element >= firstSliceElement && element < firstSliceElement + totalSliceElementsCount)
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
    ListElementsPoolContent* poolContent = NULL;

    if (elementsPool != NULL)
    {
        poolContent = (ListElementsPoolContent*)elementsPool->poolContent;

        isValid = poolContent != NULL &&
                  poolContent->elementSlices != NULL &&
                  poolContent->sliceElementIds != NULL &&
                  poolContent->totalElementsCount > 0 &&
                  poolContent->totalElementsCount >= poolContent->availableElementsCount &&
                  poolContent->slicesCount > 0 &&
                  poolContent->slicesCount <= poolContent->maximumSlicesCount;
    }

    if (isValid)
    {
        for (size_t sliceIndex = 0; sliceIndex < poolContent->slicesCount; ++sliceIndex)
        {
            if (poolContent->elementSlices[sliceIndex] == NULL)
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
    const ListElementsPoolContent* poolContent = elementsPool != NULL ? (ListElementsPoolContent*)elementsPool->poolContent : NULL;
    bool areValid = poolContent != NULL;

    if (areValid)
    {
        for (size_t sliceIndex = 0; sliceIndex < poolContent->slicesCount; ++sliceIndex)
        {
            if (!isValidSlice(poolContent->elementSlices[sliceIndex]))
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
    const ListElementsPoolContent* poolContent = NULL;

    if (elementsPool != NULL)
    {
        poolContent = (ListElementsPoolContent*)elementsPool->poolContent;
        ASSERT(poolContent != NULL, "NULL list elements pool poolContent!");
    }

    if (poolContent != NULL)
    {
        const ListElementsSlice* slice = NULL;

        ASSERT(poolContent->elementSlices != NULL, "NULL element slices!");

        if (sliceElementId.sliceIndex < poolContent->slicesCount && poolContent->elementSlices != NULL)
        {
            slice = poolContent->elementSlices[sliceElementId.sliceIndex];
            ASSERT(slice != NULL, "NULL list elements pool slice!");

            isValid = slice != NULL && sliceElementId.sliceElementIndex < slice->totalElementsCount;
        }
    }

    return isValid;
}
