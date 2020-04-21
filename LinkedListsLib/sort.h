#ifndef SORT_H
#define SORT_H

#include <stdlib.h>

#include "linkedlist.h"

#ifdef __cplusplus
extern "C"{
#endif

void insertionSortAscendingByPriority(ListElement** array, const size_t arraySize);
void insertionSortDescendingByPriority(ListElement** array, const size_t arraySize);

#ifdef __cplusplus
}
#endif

#endif // SORT_H
