#include <stdio.h>
#include <pthread.h>

#include "sort.h"
#include "listsortutils.h"

#include "../Utils/error.h"

#define SORT_ASCENDING TRUE
#define SORT_DESCENDING FALSE

typedef struct
{
    ListElement** array;
    ListElement** auxArray;
    size_t startIndex;
    size_t endIndex;

} MergeSortThreadInput;

typedef struct
{
    ListElement** array;
    size_t startIndex;
    size_t endIndex;
} QuickSortThreadInput;

// these functions are supposed to be "private", should not be accessed outside this file
void _doMergeSortByPriority(ListElement** array, const size_t arraySize, boolean isAscendingOrderRequired);
void _doMergeSortAscendingByPriority(ListElement** array, ListElement** auxArray, size_t startIndex, size_t endIndex);
void _doMergeSortDescendingByPriority(ListElement** array, ListElement** auxArray, size_t startIndex, size_t endIndex);
void _doQuickSortAscendingByPriority(ListElement** toSort, size_t beginIndex, size_t endIndex);
void _doQuickSortDescendingByPriority(ListElement** toSort, size_t beginIndex, size_t endIndex);
void _doEnhancedMergeSortByPriority(ListElement** array, const size_t arraySize, boolean isAscendingOrderRequired);
void _doEnhancedMergeSortAscendingByPriority(ListElement** array, ListElement** auxArray, const size_t startIndex, const size_t endIndex);
void _doEnhancedMergeSortDescendingByPriority(ListElement** array, ListElement** auxArray, const size_t startIndex, const size_t endIndex);
void* _wrapperEnhancedMergeSortAscendingByPriority(void* mergeSortThreadInput);
void* _wrapperEnhancedMergeSortDescendingByPriority(void* mergeSortThreadInput);
void _doQuickMergeSortByPriority(ListElement** array, const size_t arraySize, boolean isAscendingOrderRequired);
void _doQuickMergeSortAscendingByPriority(ListElement** array, ListElement** auxArray, const size_t startIndex, const size_t endIndex);
void _doQuickMergeSortDescendingByPriority(ListElement** array, ListElement** auxArray, const size_t startIndex, const size_t endIndex);
void* _wrapperQuickSortAscendingByPriority(void* quickSortThreadInput);
void* _wrapperQuickSortDescendingByPriority(void* quickSortThreadInput);

void swapElement(ListElement** first, ListElement** second)
{
    ASSERT_CONDITION(first != NULL && second != NULL, "Attempt to swap by dereferencing NULL pointer");

    ListElement* swap = *first;
    *first = *second;
    *second = swap;
}

void insertionSortAscendingByPriority(ListElement** array, const size_t arraySize)
{
    INSERTION_SORT(ASCENDING, priority)
}

void insertionSortDescendingByPriority(ListElement **array, const size_t arraySize)
{
    INSERTION_SORT(DESCENDING, priority)
}

void heapSortAscendingByPriority(ListElement** array, const size_t arraySize)
{
    HEAP_SORT(ASCENDING, priority)
}

void heapSortDescendingByPriority(ListElement** array, const size_t arraySize)
{
    HEAP_SORT(DESCENDING, priority)
}

void mergeSortAscendingByPriority(ListElement** array, const size_t arraySize)
{
    if (array != NULL && arraySize > 0)
    {
        _doMergeSortByPriority(array, arraySize, SORT_ASCENDING);
    }
}

void mergeSortDescendingByPriority(ListElement** array, const size_t arraySize)
{
    if (array != NULL && arraySize > 0)
    {
        _doMergeSortByPriority(array, arraySize, SORT_DESCENDING);
    }
}

void quickSortAscendingByPriority(ListElement** array, const size_t arraySize)
{
    if (array != NULL && arraySize > 0)
    {
        _doQuickSortAscendingByPriority(array, 0, arraySize-1);
    }
}

void quickSortDescendingByPriority(ListElement** array, const size_t arraySize)
{
    if (array != NULL && arraySize > 0)
    {
        _doQuickSortDescendingByPriority(array, 0, arraySize-1);
    }
}

void enhancedMergeSortAscendingByPriority(ListElement** array, const size_t arraySize)
{
    _doEnhancedMergeSortByPriority(array, arraySize, SORT_ASCENDING);
}

void enhancedMergeSortDescendingByPriority(ListElement** array, const size_t arraySize)
{
    _doEnhancedMergeSortByPriority(array, arraySize, SORT_DESCENDING);
}

void quickMergeSortAscendingByPriority(ListElement** array, const size_t arraySize)
{
    _doQuickMergeSortByPriority(array, arraySize, SORT_ASCENDING);
}

void quickMergeSortDescendingByPriority(ListElement** array, const size_t arraySize)
{
    _doQuickMergeSortByPriority(array, arraySize, SORT_DESCENDING);
}

// "private" functions
void _doMergeSortByPriority(ListElement** array, const size_t arraySize, boolean isAscendingOrderRequired)
{
    ListElement** auxArray = (ListElement**)calloc(arraySize, sizeof(ListElement*));

    if (auxArray != NULL)
    {
        isAscendingOrderRequired == TRUE ? _doMergeSortAscendingByPriority(array, auxArray, 0, arraySize-1) :
                         _doMergeSortDescendingByPriority(array, auxArray, 0, arraySize-1);
    }
    else
    {
        fprintf(stderr, "Cannot perform merge sort, unable to allocate memory for the auxiliary array");
    }
}

void _doMergeSortAscendingByPriority(ListElement** array, ListElement** auxArray, size_t startIndex, size_t endIndex)
{
    MERGE_SORT(ASCENDING, priority, _doMergeSortAscendingByPriority);
}

void _doMergeSortDescendingByPriority(ListElement** array, ListElement** auxArray, size_t startIndex, size_t endIndex)
{
    MERGE_SORT(DESCENDING, priority, _doMergeSortDescendingByPriority);
}

void _doQuickSortAscendingByPriority(ListElement** array, size_t beginIndex, size_t endIndex)
{
    QUICK_SORT(ASCENDING, priority, _doQuickSortAscendingByPriority);
}

void _doQuickSortDescendingByPriority(ListElement** array, size_t beginIndex, size_t endIndex)
{
    QUICK_SORT(DESCENDING, priority, _doQuickSortDescendingByPriority);
}

void _doEnhancedMergeSortByPriority(ListElement** array, const size_t arraySize, boolean isAscendingOrderRequired)
{
    ListElement** auxArray = (ListElement**)calloc(arraySize, sizeof(ListElement*));

    if (auxArray != NULL)
    {
        isAscendingOrderRequired == TRUE ? _doEnhancedMergeSortAscendingByPriority(array, auxArray, 0, arraySize-1) :
                         _doEnhancedMergeSortDescendingByPriority(array, auxArray, 0, arraySize-1);
    }
    else
    {
        fprintf(stderr, "Cannot perform enhanced merge sort, unable to allocate memory for the auxiliary array");
    }
}

void _doEnhancedMergeSortAscendingByPriority(ListElement** array, ListElement** auxArray, const size_t startIndex, const size_t endIndex)
{
    ENHANCED_MERGE_SORT(ASCENDING, priority, &_wrapperEnhancedMergeSortAscendingByPriority)
}

void _doEnhancedMergeSortDescendingByPriority(ListElement** array, ListElement** auxArray, const size_t startIndex, const size_t endIndex)
{
    ENHANCED_MERGE_SORT(DESCENDING, priority, &_wrapperEnhancedMergeSortDescendingByPriority)
}

void* _wrapperEnhancedMergeSortAscendingByPriority(void* mergeSortThreadInput)
{
    if (mergeSortThreadInput != NULL)
    {
        const MergeSortThreadInput* threadInput = (MergeSortThreadInput*)mergeSortThreadInput;
        _doMergeSortAscendingByPriority(threadInput->array, threadInput->auxArray, threadInput->startIndex, threadInput->endIndex);
    }

    return NULL;
}

void* _wrapperEnhancedMergeSortDescendingByPriority(void* mergeSortThreadInput)
{
    if (mergeSortThreadInput != NULL)
    {
        const MergeSortThreadInput* threadInput = (MergeSortThreadInput*)mergeSortThreadInput;
        _doMergeSortDescendingByPriority(threadInput->array, threadInput->auxArray, threadInput->startIndex, threadInput->endIndex);
    }

    return NULL;
}

void _doQuickMergeSortByPriority(ListElement** array, const size_t arraySize, boolean isAscendingOrderRequired)
{
    ListElement** auxArray = (ListElement**)calloc(arraySize, sizeof(ListElement*));

    if (auxArray != NULL)
    {
        isAscendingOrderRequired == TRUE ? _doQuickMergeSortAscendingByPriority(array, auxArray, 0, arraySize-1)
                                         : _doQuickMergeSortDescendingByPriority(array, auxArray, 0, arraySize-1);
    }
    else
    {
        fprintf(stderr, "Cannot perform enhanced merge sort, unable to allocate memory for the auxiliary array");
    }
}

void _doQuickMergeSortAscendingByPriority(ListElement** array, ListElement** auxArray, const size_t startIndex, const size_t endIndex)
{
    QUICK_MERGE_SORT(ASCENDING, priority, &_wrapperQuickSortAscendingByPriority)
}

void _doQuickMergeSortDescendingByPriority(ListElement** array, ListElement** auxArray, const size_t startIndex, const size_t endIndex)
{
    QUICK_MERGE_SORT(DESCENDING, priority, &_wrapperQuickSortDescendingByPriority)
}

void* _wrapperQuickSortAscendingByPriority(void* quickSortThreadInput)
{
    if (quickSortThreadInput != NULL)
    {
        const QuickSortThreadInput* threadInput = (QuickSortThreadInput*)quickSortThreadInput;
        _doQuickSortAscendingByPriority(threadInput->array, threadInput->startIndex, threadInput->endIndex);
    }

    return NULL;
}
void* _wrapperQuickSortDescendingByPriority(void* quickSortThreadInput)
{
    if (quickSortThreadInput != NULL)
    {
        const QuickSortThreadInput* threadInput = (QuickSortThreadInput*)quickSortThreadInput;
        _doQuickSortDescendingByPriority(threadInput->array, threadInput->startIndex, threadInput->endIndex);
    }

    return NULL;
}
