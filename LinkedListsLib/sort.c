#include "sort.h"
#include "listutils.h"
#include "../Utils/error.h"
#include <stdio.h>

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

void heapSortDescendingByPriority(ListElement **array, const size_t arraySize)
{
    HEAP_SORT(DESCENDING, priority)
}
