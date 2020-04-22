#include <stdio.h>

#include "sort.h"
#include "listsortutils.h"

#include "../Utils/error.h"

#define SORT_ASCENDING 1
#define SORT_DESCENDING 0

// these functions are supposed to be "private", should not be accessed outside this file
void _doMergeSortByPriority(ListElement** array, const size_t arraySize, int ascending);
void _doMergeSortAscendingByPriority(ListElement **array, ListElement **auxArray, size_t startIndex, size_t endIndex);
void _doMergeSortDescendingByPriority(ListElement **array, ListElement **auxArray, size_t startIndex, size_t endIndex);

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

// "private" functions
void _doMergeSortByPriority(ListElement** array, const size_t arraySize, int ascending)
{
    ListElement** auxArray = (ListElement**)calloc(arraySize, sizeof(ListElement*));

    if (auxArray != NULL)
    {
        ascending != 0 ? _doMergeSortAscendingByPriority(array, auxArray, 0, arraySize-1) :
                         _doMergeSortDescendingByPriority(array, auxArray, 0, arraySize-1);
    }
    else
    {
        fprintf(stderr, "Cannot perform merge sort, unable to allocate memory for the auxiliary array");
    }
}

void _doMergeSortAscendingByPriority(ListElement **array, ListElement **auxArray, size_t startIndex, size_t endIndex)
{
    MERGE_SORT(ASCENDING, priority, _doMergeSortAscendingByPriority);
}

void _doMergeSortDescendingByPriority(ListElement **array, ListElement **auxArray, size_t startIndex, size_t endIndex)
{
    MERGE_SORT(DESCENDING, priority, _doMergeSortDescendingByPriority);
}
