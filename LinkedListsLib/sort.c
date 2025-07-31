#include <stdio.h>

#include "sort.h"
#include "listsortutils.h"

#include "error.h"

#define SORT_ASCENDING true
#define SORT_DESCENDING false

#ifdef UNIX_OS

#include <pthread.h>

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

#endif

// these functions are supposed to be "private", should not be accessed outside this file
static void _doMergeSortByPriority(ListElement** array, const size_t arraySize, bool isAscendingOrderRequired);
static void _doMergeSortAscendingByPriority(ListElement** array, ListElement** auxArray, size_t startIndex, size_t endIndex);
static void _doMergeSortDescendingByPriority(ListElement** array, ListElement** auxArray, size_t startIndex, size_t endIndex);
static void _doQuickSortAscendingByPriority(ListElement** toSort, size_t beginIndex, size_t endIndex);
static void _doQuickSortDescendingByPriority(ListElement** toSort, size_t beginIndex, size_t endIndex);

#ifdef UNIX_OS
static void _doEnhancedMergeSortByPriority(ListElement** array, const size_t arraySize, bool isAscendingOrderRequired);
static void _doEnhancedMergeSortAscendingByPriority(ListElement** array, ListElement** auxArray, const size_t startIndex, const size_t endIndex);
static void _doEnhancedMergeSortDescendingByPriority(ListElement** array, ListElement** auxArray, const size_t startIndex, const size_t endIndex);
static void* _wrapperEnhancedMergeSortAscendingByPriority(void* mergeSortThreadInput);
static void* _wrapperEnhancedMergeSortDescendingByPriority(void* mergeSortThreadInput);
static void _doQuickMergeSortByPriority(ListElement** array, const size_t arraySize, bool isAscendingOrderRequired);
static void _doQuickMergeSortAscendingByPriority(ListElement** array, ListElement** auxArray, const size_t startIndex, const size_t endIndex);
static void _doQuickMergeSortDescendingByPriority(ListElement** array, ListElement** auxArray, const size_t startIndex, const size_t endIndex);
static void* _wrapperQuickSortAscendingByPriority(void* quickSortThreadInput);
static void* _wrapperQuickSortDescendingByPriority(void* quickSortThreadInput);
static void _doEnhancedQuickSortAscendingByPriority(ListElement** array, size_t beginIndex, size_t endIndex);
static void _doEnhancedQuickSortDescendingByPriority(ListElement** array, size_t beginIndex, size_t endIndex);
#endif

void swapElement(ListElement** first, ListElement** second)
{
    if (first != NULL && second != NULL)
    {
        ListElement* swap = *first;
        *first = *second;
        *second = swap;
    }
    else
    {
        ASSERT(false, "Attempt to swap by dereferencing NULL pointer");
    }
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

#ifdef UNIX_OS
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

void enhancedQuickSortAscendingByPriority(ListElement** array, const size_t arraySize)
{
    if (array != NULL && arraySize > 0)
    {
        _doEnhancedQuickSortAscendingByPriority(array, 0, arraySize-1);
    }
}

void enhancedQuickSortDescendingByPriority(ListElement** array, const size_t arraySize)
{
    if (array != NULL && arraySize > 0)
    {
        _doEnhancedQuickSortDescendingByPriority(array, 0, arraySize-1);
    }
}
#endif

// "private" functions
static void _doMergeSortByPriority(ListElement** array, const size_t arraySize, bool isAscendingOrderRequired)
{
    ListElement** auxArray = (ListElement**)calloc(arraySize, sizeof(ListElement*));

    if (auxArray != NULL)
    {
        isAscendingOrderRequired ? _doMergeSortAscendingByPriority(array, auxArray, 0, arraySize-1)
                                 : _doMergeSortDescendingByPriority(array, auxArray, 0, arraySize-1);

        free(auxArray);
        auxArray = NULL;
    }
    else
    {
        fprintf(stderr, "Cannot perform merge sort, unable to allocate memory for the auxiliary array");
    }
}

static void _doMergeSortAscendingByPriority(ListElement** array, ListElement** auxArray, size_t startIndex, size_t endIndex)
{
    MERGE_SORT(ASCENDING, priority, _doMergeSortAscendingByPriority);
}

static void _doMergeSortDescendingByPriority(ListElement** array, ListElement** auxArray, size_t startIndex, size_t endIndex)
{
    MERGE_SORT(DESCENDING, priority, _doMergeSortDescendingByPriority);
}

static void _doQuickSortAscendingByPriority(ListElement** array, size_t beginIndex, size_t endIndex)
{
    QUICK_SORT(ASCENDING, priority, _doQuickSortAscendingByPriority);
}

static void _doQuickSortDescendingByPriority(ListElement** array, size_t beginIndex, size_t endIndex)
{
    QUICK_SORT(DESCENDING, priority, _doQuickSortDescendingByPriority);
}

#ifdef UNIX_OS
static void _doEnhancedMergeSortByPriority(ListElement** array, const size_t arraySize, bool isAscendingOrderRequired)
{
    ListElement** auxArray = (ListElement**)calloc(arraySize, sizeof(ListElement*));

    if (auxArray != NULL)
    {
        isAscendingOrderRequired ? _doEnhancedMergeSortAscendingByPriority(array, auxArray, 0, arraySize-1)
                                 : _doEnhancedMergeSortDescendingByPriority(array, auxArray, 0, arraySize-1);

        free(auxArray);
        auxArray = NULL;
    }
    else
    {
        fprintf(stderr, "Cannot perform enhanced merge sort, unable to allocate memory for the auxiliary array");
    }
}

static void _doEnhancedMergeSortAscendingByPriority(ListElement** array, ListElement** auxArray, const size_t startIndex, const size_t endIndex)
{
    ENHANCED_MERGE_SORT(ASCENDING, priority, &_wrapperEnhancedMergeSortAscendingByPriority)
}

static void _doEnhancedMergeSortDescendingByPriority(ListElement** array, ListElement** auxArray, const size_t startIndex, const size_t endIndex)
{
    ENHANCED_MERGE_SORT(DESCENDING, priority, &_wrapperEnhancedMergeSortDescendingByPriority)
}

static void* _wrapperEnhancedMergeSortAscendingByPriority(void* mergeSortThreadInput)
{
    if (mergeSortThreadInput != NULL)
    {
        const MergeSortThreadInput* threadInput = (MergeSortThreadInput*)mergeSortThreadInput;
        _doMergeSortAscendingByPriority(threadInput->array, threadInput->auxArray, threadInput->startIndex, threadInput->endIndex);
    }

    return NULL;
}

static void* _wrapperEnhancedMergeSortDescendingByPriority(void* mergeSortThreadInput)
{
    if (mergeSortThreadInput != NULL)
    {
        const MergeSortThreadInput* threadInput = (MergeSortThreadInput*)mergeSortThreadInput;
        _doMergeSortDescendingByPriority(threadInput->array, threadInput->auxArray, threadInput->startIndex, threadInput->endIndex);
    }

    return NULL;
}

static void _doQuickMergeSortByPriority(ListElement** array, const size_t arraySize, bool isAscendingOrderRequired)
{
    ListElement** auxArray = (ListElement**)calloc(arraySize, sizeof(ListElement*));

    if (auxArray != NULL)
    {
        isAscendingOrderRequired ? _doQuickMergeSortAscendingByPriority(array, auxArray, 0, arraySize-1)
                                 : _doQuickMergeSortDescendingByPriority(array, auxArray, 0, arraySize-1);

        free(auxArray);
        auxArray = NULL;
    }
    else
    {
        fprintf(stderr, "Cannot perform enhanced merge sort, unable to allocate memory for the auxiliary array");
    }
}

static void _doQuickMergeSortAscendingByPriority(ListElement** array, ListElement** auxArray, const size_t startIndex, const size_t endIndex)
{
    QUICK_MERGE_SORT(ASCENDING, priority, &_wrapperQuickSortAscendingByPriority)
}

static void _doQuickMergeSortDescendingByPriority(ListElement** array, ListElement** auxArray, const size_t startIndex, const size_t endIndex)
{
    QUICK_MERGE_SORT(DESCENDING, priority, &_wrapperQuickSortDescendingByPriority)
}

static void* _wrapperQuickSortAscendingByPriority(void* quickSortThreadInput)
{
    if (quickSortThreadInput != NULL)
    {
        const QuickSortThreadInput* threadInput = (QuickSortThreadInput*)quickSortThreadInput;
        _doQuickSortAscendingByPriority(threadInput->array, threadInput->startIndex, threadInput->endIndex);
    }

    return NULL;
}
static void* _wrapperQuickSortDescendingByPriority(void* quickSortThreadInput)
{
    if (quickSortThreadInput != NULL)
    {
        const QuickSortThreadInput* threadInput = (QuickSortThreadInput*)quickSortThreadInput;
        _doQuickSortDescendingByPriority(threadInput->array, threadInput->startIndex, threadInput->endIndex);
    }

    return NULL;
}

static void _doEnhancedQuickSortAscendingByPriority(ListElement** array, size_t beginIndex, size_t endIndex)
{
    ENHANCED_QUICK_SORT(ASCENDING, priority, &_wrapperQuickSortAscendingByPriority, _doQuickSortAscendingByPriority)
}


static void _doEnhancedQuickSortDescendingByPriority(ListElement** array, size_t beginIndex, size_t endIndex)
{
    ENHANCED_QUICK_SORT(DESCENDING, priority, &_wrapperQuickSortDescendingByPriority, _doQuickSortDescendingByPriority)
}
#endif
