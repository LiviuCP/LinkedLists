#ifndef SORT_H
#define SORT_H

#include <stdlib.h>

#include "listelement.h"

#ifdef __cplusplus
extern "C"{
#endif

void insertionSortAscendingByPriority(ListElement** array, const size_t arraySize);
void insertionSortDescendingByPriority(ListElement** array, const size_t arraySize);
void heapSortAscendingByPriority(ListElement** array, const size_t arraySize);
void heapSortDescendingByPriority(ListElement** array, const size_t arraySize);
void mergeSortAscendingByPriority(ListElement** array, const size_t arraySize);
void mergeSortDescendingByPriority(ListElement** array, const size_t arraySize);
void quickSortAscendingByPriority(ListElement** array, const size_t arraySize);
void quickSortDescendingByPriority(ListElement** array, const size_t arraySize);

#ifdef UNIX_OS
void enhancedMergeSortAscendingByPriority(ListElement** array, const size_t arraySize);
void enhancedMergeSortDescendingByPriority(ListElement** array, const size_t arraySize);
void quickMergeSortAscendingByPriority(ListElement** array, const size_t arraySize);
void quickMergeSortDescendingByPriority(ListElement** array, const size_t arraySize);
void enhancedQuickSortAscendingByPriority(ListElement** array, const size_t arraySize);
void enhancedQuickSortDescendingByPriority(ListElement** array, const size_t arraySize);
#endif

#ifdef __cplusplus
}
#endif

#endif // SORT_H
