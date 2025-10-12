#include <stdio.h>
#include <stdint.h>

#include "listelementspool.h"
#include "bitoperations.h"
#include "error.h"


#define DEFAULT_MAX_SLICES_COUNT 8
#define SLICE_OFFSET 4

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
static bool initListElementsPool(ListElementsPool* elementsPool, size_t maxSlicesCount);
static void addSliceToElementsPool(ListElementsPool* elementsPool);
static void deleteUnusedSlices(ListElementsPool* elementsPool);
static bool retrieveSliceIndex(const ListElement* element, const ListElementsPool* elementsPool, size_t* sliceIndex);
static ListElementsSlice* createSlice(size_t elementsCount);
static void deleteSlice(ListElementsSlice* slice);

ListElementsPool* createListElementsPool(size_t maxSlicesCount)
{
    ListElementsPool* elementsPool = (ListElementsPool*)malloc(sizeof(ListElementsPool));

    if (elementsPool != NULL)
    {
        const bool success = initListElementsPool(elementsPool, maxSlicesCount);

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
    ListElementsPoolContent* poolContent = elementsPool != NULL ? (ListElementsPoolContent*)elementsPool->poolContent : NULL;
    ListElementsSlice** elementSlices = poolContent != NULL ? poolContent->elementSlices : NULL;
    SliceElementId* sliceElementIds = poolContent != NULL ? poolContent->sliceElementIds : NULL;
    const size_t slicesCount = poolContent != NULL ? poolContent->slicesCount : 0;

    ASSERT(elementsPool == NULL || (elementSlices != NULL && sliceElementIds != NULL && slicesCount > 0), "Invalid pool content!");
    FREE(elementsPool);
    FREE(poolContent);

    if (elementSlices != NULL)
    {
        for (size_t sliceIndex = 0; sliceIndex < slicesCount; ++sliceIndex)
        {
            ASSERT(elementSlices[sliceIndex] != NULL, "NULL element slice!");
            deleteSlice(elementSlices[sliceIndex]);
            elementSlices[sliceIndex] = NULL;
        }

        free(elementSlices);
        elementSlices = NULL;
    }

    FREE(sliceElementIds);
}

ListElement* aquireElement(ListElementsPool* elementsPool)
{
    ListElement* aquiredElement = NULL;
    ListElementsPoolContent* poolContent = elementsPool != NULL ? elementsPool->poolContent : NULL;

    if (poolContent != NULL && poolContent->availableElementsCount == 0)
    {
        addSliceToElementsPool(elementsPool);
    }

    ListElementsSlice** elementSlices = poolContent != NULL ? poolContent->elementSlices : NULL;
    SliceElementId* sliceElementIds = poolContent != NULL ? poolContent->sliceElementIds : NULL;

    ASSERT(elementsPool == NULL || (elementSlices != NULL && sliceElementIds != NULL), "Invalid list elements pool content!");

    if (elementSlices != NULL && sliceElementIds != NULL && poolContent->availableElementsCount > 0)
    {
        ListElementsSlice* slice = NULL;
        ListElement* lastAvailableSliceElement = NULL;

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

    return aquiredElement;
}

bool aquireElements(ListElementsPool* elementsPool, ListElement** elements, size_t requiredElementsCount)
{
    bool success = false;
    ListElementsPoolContent* poolContent = elementsPool != NULL ? (ListElementsPoolContent*)elementsPool->poolContent : NULL;
    bool canRequiredElementsCountBeAquired = false;

    if (poolContent != NULL && elements != NULL && requiredElementsCount > 0)
    {
        // the maximum number of elements to be aquired cannot exceed 2 slices, i.e. maximum one slice can be created per aquiring operation provided the maximum slices count hasn't been reached
        const size_t maxAllowedCountToBeAquired = ((poolContent->availableElementsCount < ELEMENTS_POOL_SLICE_SIZE ? poolContent->availableElementsCount
                                                                                                                   : ELEMENTS_POOL_SLICE_SIZE) + ELEMENTS_POOL_SLICE_SIZE);
        if (requiredElementsCount <= maxAllowedCountToBeAquired)
        {
            if (requiredElementsCount > poolContent->availableElementsCount)
            {
                addSliceToElementsPool(elementsPool);
            }

            canRequiredElementsCountBeAquired = poolContent->availableElementsCount >= requiredElementsCount;
        }
    }

    ListElementsSlice** elementSlices = poolContent != NULL ? poolContent->elementSlices : NULL;
    SliceElementId* sliceElementIds = poolContent != NULL ? poolContent->sliceElementIds : NULL;

    ASSERT(elementsPool == NULL || (elementSlices != NULL && sliceElementIds != NULL), "Invalid list elements pool content!");

    if (canRequiredElementsCountBeAquired && elementSlices != NULL && sliceElementIds != NULL)
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
    ListElementsPoolContent* poolContent = elementsPool != NULL ? (ListElementsPoolContent*)elementsPool->poolContent : NULL;
    ListElementsSlice** elementSlices = poolContent != NULL ? poolContent->elementSlices : NULL;
    SliceElementId* sliceElementIds = poolContent != NULL ? poolContent->sliceElementIds : NULL;

    ASSERT(elementsPool == NULL || (elementSlices != NULL && sliceElementIds != NULL), "Invalid list elements pool content!");

    size_t sliceIndex = 0;
    const bool isElementAddressValid = element != NULL && elementSlices != NULL && sliceElementIds != NULL ? retrieveSliceIndex(element, elementsPool, &sliceIndex) : false;
    ListElementsSlice* slice = isElementAddressValid ? elementSlices[sliceIndex] : NULL;

    ListElement* sliceElements = slice != NULL ? slice->elements : NULL;
    byte_t* availabilityFlags = slice != NULL ? slice->availabilityFlags : NULL;

    ASSERT(slice == NULL || (sliceElements != NULL && availabilityFlags != NULL), "Invalid slice content!");

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

void shrinkPoolCapacity(ListElementsPool* elementsPool)
{
    ListElementsPoolContent* poolContent = elementsPool != NULL ? (ListElementsPoolContent*)elementsPool->poolContent : NULL;
    ASSERT(elementsPool == NULL || poolContent != NULL, "Invalid list elements pool content!");

    // some available elements should remain after deleting unused slices
    if (poolContent != NULL && poolContent->availableElementsCount > ELEMENTS_POOL_SLICE_SIZE)
    {
        deleteUnusedSlices(elementsPool);
    }
}

size_t getAvailableElementsCount(ListElementsPool* elementsPool)
{
    const ListElementsPoolContent* poolContent = elementsPool != NULL ? (ListElementsPoolContent*)elementsPool->poolContent : NULL;
    ASSERT(elementsPool == NULL || poolContent != NULL, "Invalid list elements pool content!");

    return poolContent != NULL ? poolContent->availableElementsCount : 0;
}

size_t getAquiredElementsCount(ListElementsPool* elementsPool)
{
    const ListElementsPoolContent* poolContent = elementsPool != NULL ? (ListElementsPoolContent*)elementsPool->poolContent : NULL;
    const size_t availableElementsCount = poolContent != NULL ? poolContent->availableElementsCount : 0;
    const size_t totalElementsCount = poolContent != NULL ? poolContent->totalElementsCount : 0;

    ASSERT(elementsPool == NULL || poolContent != NULL && totalElementsCount >= availableElementsCount, "Invalid list elements pool content!");

    const size_t aquiredElementsCount = totalElementsCount >= availableElementsCount ? totalElementsCount - availableElementsCount : 0;

    return aquiredElementsCount;
}

static bool initListElementsPool(ListElementsPool* elementsPool, size_t maxSlicesCount)
{
    static_assert(ELEMENTS_POOL_SLICE_SIZE > 0 && ELEMENTS_POOL_SLICE_SIZE % BYTE_SIZE == 0, "Invalid slice size!");
    static_assert(DEFAULT_MAX_SLICES_COUNT > 0, "The default number of slices should not be 0!");

    bool success = false;

    SliceElementId* sliceElementIds = NULL;
    const size_t totalElementsCount = ELEMENTS_POOL_SLICE_SIZE;
    const size_t maximumSlicesCount = maxSlicesCount == 0 ? DEFAULT_MAX_SLICES_COUNT : maxSlicesCount;

    ListElementsPoolContent* poolContent = elementsPool != NULL ? malloc(sizeof(ListElementsPoolContent)) : NULL;
    ListElementsSlice** elementSlices = poolContent != NULL ? (ListElementsSlice**)malloc(maximumSlicesCount * sizeof(ListElementsSlice*)) : NULL;

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

static void addSliceToElementsPool(ListElementsPool* elementsPool)
{
    ListElementsPoolContent* poolContent = elementsPool != NULL ? (ListElementsPoolContent*)elementsPool->poolContent : NULL;
    ListElementsSlice** elementSlices = poolContent != NULL ? poolContent->elementSlices : NULL;
    SliceElementId* sliceElementIds = poolContent != NULL ? poolContent->sliceElementIds : NULL;
    const size_t totalElementsCount = poolContent != NULL ? poolContent->totalElementsCount : 0;

    ASSERT(elementsPool == NULL || (poolContent != NULL && elementSlices != NULL && sliceElementIds != NULL && poolContent->availableElementsCount <= totalElementsCount),
           "Invalid elements pool content!");

    const bool canCreateNewSlice = elementSlices != NULL && sliceElementIds != NULL && poolContent->slicesCount < poolContent->maximumSlicesCount;
    ListElementsSlice* newSlice = canCreateNewSlice ? createSlice(ELEMENTS_POOL_SLICE_SIZE) : NULL;
    const size_t newTotalElementsCount = newSlice != NULL ? totalElementsCount + newSlice->totalElementsCount : totalElementsCount;
    SliceElementId* newSliceElementIds = newSlice != NULL ? (SliceElementId*)realloc(sliceElementIds, newTotalElementsCount * sizeof(SliceElementId)) : NULL;

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
    }
    else
    {
        deleteSlice(newSlice);
        newSlice = NULL;
    }
}

static void deleteUnusedSlices(ListElementsPool* elementsPool)
{
    ListElementsPoolContent* poolContent = elementsPool != NULL ? (ListElementsPoolContent*)elementsPool->poolContent : NULL;
    ListElementsSlice** elementSlices = poolContent != NULL ? poolContent->elementSlices : NULL;
    SliceElementId* sliceElementIds = poolContent != NULL ? poolContent->sliceElementIds : NULL;
    const size_t slicesCount = poolContent != NULL ? poolContent->slicesCount : 0;
    size_t nrOfSlicesLeftAfterRemoval = slicesCount;
    size_t nrOfElementsLeftAfterRemoval = poolContent ? poolContent->totalElementsCount : 0;

    ASSERT(elementsPool == NULL || (poolContent != NULL && elementSlices != NULL && sliceElementIds != NULL && poolContent->slicesCount > 0), "Invalid elements pool content!");

    // at least one slice should remain after cleanup
    size_t currentSliceIndex = poolContent != NULL && poolContent->availableElementsCount == poolContent->totalElementsCount ? 1 : 0;

    while (currentSliceIndex < nrOfSlicesLeftAfterRemoval)
    {
        ListElementsSlice* currentSlice = elementSlices[currentSliceIndex];

        if (currentSlice == NULL || currentSlice->availabilityFlags == NULL)
        {
            ASSERT(false, "Invalid slice!");
            break;
        }

        if (currentSlice->availableElementsCount == currentSlice->totalElementsCount)
        {
            if (nrOfElementsLeftAfterRemoval < currentSlice->totalElementsCount)
            {
                ASSERT(false, "");
                break;
            }

            nrOfElementsLeftAfterRemoval -= currentSlice->totalElementsCount;
            deleteSlice(currentSlice);
            const size_t lastSliceIndex = nrOfSlicesLeftAfterRemoval - 1;
            --nrOfSlicesLeftAfterRemoval;

            if (lastSliceIndex > currentSliceIndex)
            {
                elementSlices[currentSliceIndex] = elementSlices[lastSliceIndex];
                elementSlices[lastSliceIndex] = NULL;
                continue;
            }

            break;
        }

        ASSERT(currentSlice->availableElementsCount < currentSlice->totalElementsCount, "Available slice elements count should not exceed the total slice elements count!");
        ++currentSliceIndex;
    }

    SliceElementId* newSliceElementIds = nrOfSlicesLeftAfterRemoval < slicesCount ? realloc(sliceElementIds, nrOfElementsLeftAfterRemoval * sizeof(SliceElementId)) : NULL;

    // if slice element IDs cannot be resized the old slice element IDs remain (still valid)
    if (newSliceElementIds != NULL)
    {
        size_t sliceElementIdIndex = 0;

        for (size_t sliceIndex = 0; sliceIndex < nrOfSlicesLeftAfterRemoval; ++sliceIndex)
        {
            ListElementsSlice* slice = elementSlices[sliceIndex];

            if (slice == NULL || slice->availabilityFlags == NULL)
            {
                ASSERT(false, "Invalid slice!");
                break;
            }

            for (size_t sliceElementIndex = 0; sliceElementIndex < slice->totalElementsCount; ++sliceElementIndex)
            {
                const size_t byteIndex = sliceElementIndex / BYTE_SIZE;
                const size_t bitIndex = BYTE_SIZE - 1 - sliceElementIndex % BYTE_SIZE; // bits are numbered from byte end (least significant: 0) to beginning (most significant: 7)
                const byte_t elementBitMask = LSB_MASK << bitIndex;
                const bool canElementBeAquired = slice->availabilityFlags[byteIndex] & elementBitMask; // test element availability bit

                if (canElementBeAquired)
                {
                    newSliceElementIds[sliceElementIdIndex].sliceIndex = sliceIndex;
                    newSliceElementIds[sliceElementIdIndex].sliceElementIndex = sliceElementIndex;
                    ++sliceElementIdIndex;
                }
            }
        }

        poolContent->sliceElementIds = newSliceElementIds;
    }

    if (nrOfSlicesLeftAfterRemoval < slicesCount)
    {
        poolContent->slicesCount = nrOfSlicesLeftAfterRemoval;
        const size_t elementsCountDelta = poolContent->totalElementsCount - nrOfElementsLeftAfterRemoval;
        poolContent->totalElementsCount = nrOfElementsLeftAfterRemoval;
        ASSERT(poolContent->availableElementsCount >= elementsCountDelta, "Invalid available elements count!");
        poolContent->availableElementsCount = poolContent->availableElementsCount >= elementsCountDelta ? poolContent->availableElementsCount - elementsCountDelta : 0;
    }
}

static bool retrieveSliceIndex(const ListElement* element, const ListElementsPool* elementsPool, size_t* sliceIndex)
{
    bool isValid = false;

    const ListElementsPoolContent* poolContent = elementsPool != NULL ? (ListElementsPoolContent*)elementsPool->poolContent : NULL;
    ListElementsSlice** elementSlices = poolContent != NULL ? poolContent->elementSlices : NULL;

    ASSERT(elementsPool == NULL || elementSlices != NULL, "Invalid list elements pool content!");

    const size_t nrOfSlicesToCheck = elementSlices != NULL && element != NULL ? poolContent->slicesCount : 0;

    for (size_t index = 0; index < nrOfSlicesToCheck; ++index)
    {
        const ListElementsSlice* slice = elementSlices[index];

        if (slice == NULL)
        {
            ASSERT(false, "NULL slice!");
            break;
        }

        const ListElement* firstSliceElement = slice->elements;

        if (element >= firstSliceElement && element < firstSliceElement + slice->totalElementsCount)
        {
            isValid = true;

            if (sliceIndex != NULL)
            {
                *sliceIndex = index;
            }

            break;
        }
    }

    return isValid;
}

static ListElementsSlice* createSlice(size_t elementsCount)
{
    ListElementsSlice* slice = NULL;
    void* data = NULL;

    const bool isValidElementsCount = elementsCount > 0 && elementsCount % BYTE_SIZE == 0;
    ASSERT(isValidElementsCount, "Invalid elements count for requested slice!");

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
