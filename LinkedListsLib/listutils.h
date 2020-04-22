#ifndef LISTUTILS_H
#define LISTUTILS_H

#define ASCENDING >
#define DESCENDING <

#define SORT_LIST(condition, parameter)                                                                                                         \
{                                                                                                                                               \
    if (list != NULL && list->first != NULL && list->first->next != NULL)                                                                       \
    {                                                                                                                                           \
        /* bubble sort used due to the access model of the list elements (non-constant, element after element) */                               \
        for (;;)                                                                                                                                \
        {                                                                                                                                       \
            ListElement* currentElement = list->first;                                                                                          \
            ListElement* previousElement = list->first;                                                                                         \
            int wasSortingPerformed = 0;                                                                                                        \
                                                                                                                                                \
            /* adjust the previousElement and currentElement positions for the first two elements prior to passing through the others */        \
            /* (previousElement always one position before currentElement) */                                                                   \
            if (list->first->parameter condition list->first->next->parameter)                                                                  \
            {                                                                                                                                   \
                previousElement = list->first->next;                                                                                            \
                list->first = list->first->next;                                                                                                \
                ListElement* temp = currentElement->next->next;                                                                                 \
                currentElement->next->next = currentElement;                                                                                    \
                currentElement->next = temp;                                                                                                    \
                wasSortingPerformed = 1;                                                                                                        \
            }                                                                                                                                   \
            else                                                                                                                                \
            {                                                                                                                                   \
                currentElement = currentElement->next;                                                                                          \
            }                                                                                                                                   \
                                                                                                                                                \
            /* complete iteration by sorting the remaining elements */                                                                          \
            while (currentElement != NULL)                                                                                                      \
            {                                                                                                                                   \
                if (currentElement->next != NULL && currentElement->parameter condition currentElement->next->parameter)                        \
                {                                                                                                                               \
                    previousElement->next = currentElement->next;                                                                               \
                    ListElement* temp = currentElement->next->next;                                                                             \
                    currentElement->next->next = currentElement;                                                                                \
                    currentElement->next = temp;                                                                                                \
                    wasSortingPerformed = 1;                                                                                                    \
                    previousElement = previousElement->next;                                                                                    \
                }                                                                                                                               \
                else                                                                                                                            \
                {                                                                                                                               \
                    previousElement = currentElement;                                                                                           \
                    currentElement = currentElement->next;                                                                                      \
                }                                                                                                                               \
            }                                                                                                                                   \
                                                                                                                                                \
            if (!wasSortingPerformed)                                                                                                           \
            {                                                                                                                                   \
                break;                                                                                                                          \
            }                                                                                                                                   \
        }                                                                                                                                       \
    }                                                                                                                                           \
}

#define CHECK_IF_SORTED(condition, parameter)                                                                                                   \
{                                                                                                                                               \
    int isSorted = 1;                                                                                                                           \
                                                                                                                                                \
    if (list!= NULL)                                                                                                                            \
    {                                                                                                                                           \
        if (list->first != NULL)                                                                                                                \
        {                                                                                                                                       \
            ListElement* currentElement = list->first;                                                                                          \
            while (currentElement->next != NULL)                                                                                                \
            {                                                                                                                                   \
                if (currentElement->parameter condition currentElement->next->parameter)                                                        \
                {                                                                                                                               \
                    isSorted = 0;                                                                                                               \
                    break;                                                                                                                      \
                }                                                                                                                               \
                currentElement = currentElement->next;                                                                                          \
            }                                                                                                                                   \
        }                                                                                                                                       \
    }                                                                                                                                           \
    else                                                                                                                                        \
    {                                                                                                                                           \
        isSorted = 0;                                                                                                                           \
    }                                                                                                                                           \
                                                                                                                                                \
    return isSorted;                                                                                                                            \
}

#define INSERTION_SORT(condition, parameter)                                                                                                    \
{                                                                                                                                               \
    if (array != NULL && arraySize > 0)                                                                                                         \
    {                                                                                                                                           \
        for (int index = 1; index<(int)arraySize; ++index)                                                                                      \
        {                                                                                                                                       \
            int elementToInsert = index;                                                                                                        \
                                                                                                                                                \
            for (int checkedIndex = index-1; checkedIndex>=0; --checkedIndex)                                                                   \
            {                                                                                                                                   \
                if (array[checkedIndex]->parameter condition array[elementToInsert]->parameter)                                                 \
                {                                                                                                                               \
                    swapElement(&array[checkedIndex], &array[elementToInsert]);                                                                 \
                    elementToInsert = checkedIndex;                                                                                             \
                }                                                                                                                               \
                else                                                                                                                            \
                {                                                                                                                               \
                    break;                                                                                                                      \
                }                                                                                                                               \
            }                                                                                                                                   \
        }                                                                                                                                       \
    }                                                                                                                                           \
}

#define BUILD_HEAP(condition, parameter)                                                                                                        \
{                                                                                                                                               \
    for (int index = 0; index<(int)arraySize; ++index)                                                                                          \
    {                                                                                                                                           \
        int checkedElementIndex = index;                                                                                                        \
        while(checkedElementIndex > 0)                                                                                                          \
        {                                                                                                                                       \
            int parentElementIndex = (checkedElementIndex-1)/2;                                                                                 \
            if (array[checkedElementIndex]->parameter condition array[parentElementIndex]->parameter)                                           \
            {                                                                                                                                   \
                swapElement(&array[parentElementIndex], &array[checkedElementIndex]);                                                           \
                checkedElementIndex = parentElementIndex;                                                                                       \
            }                                                                                                                                   \
            else                                                                                                                                \
            {                                                                                                                                   \
                break;                                                                                                                          \
            }                                                                                                                                   \
        }                                                                                                                                       \
    }                                                                                                                                           \
}

#define HEAP_SORT(condition, parameter)                                                                                                         \
{                                                                                                                                               \
    BUILD_HEAP(condition, parameter)                                                                                                            \
    int currentToSortIndex = (int)arraySize-1;                                                                                                  \
    while(currentToSortIndex>0)                                                                                                                 \
    {                                                                                                                                           \
        swapElement(&array[0], &array[currentToSortIndex]);                                                                                     \
        --currentToSortIndex;                                                                                                                   \
        int checkedElementIndex = 0;                                                                                                            \
        while (currentToSortIndex > checkedElementIndex)                                                                                        \
        {                                                                                                                                       \
            int leftChildIndex = checkedElementIndex+checkedElementIndex+1;                                                                     \
            if (currentToSortIndex > leftChildIndex)                                                                                            \
            {                                                                                                                                   \
                int rightChildIndex = leftChildIndex+1;                                                                                         \
                int toSwapIndex = leftChildIndex;                                                                                               \
                if (array[rightChildIndex]->parameter condition array[leftChildIndex]->parameter)                                               \
                {                                                                                                                               \
                    toSwapIndex = rightChildIndex;                                                                                              \
                }                                                                                                                               \
                if (array[toSwapIndex]->parameter condition array[checkedElementIndex]->parameter)                                              \
                {                                                                                                                               \
                    swapElement(&array[checkedElementIndex], &array[toSwapIndex]);                                                              \
                    checkedElementIndex = toSwapIndex;                                                                                          \
                }                                                                                                                               \
                else                                                                                                                            \
                {                                                                                                                               \
                    break;                                                                                                                      \
                }                                                                                                                               \
            }                                                                                                                                   \
            else if (leftChildIndex == currentToSortIndex)                                                                                      \
            {                                                                                                                                   \
                if (array[leftChildIndex]->parameter condition array[checkedElementIndex]->parameter)                                           \
                {                                                                                                                               \
                    swapElement(&array[checkedElementIndex], &array[leftChildIndex]);                                                           \
                }                                                                                                                               \
                break;                                                                                                                          \
            }                                                                                                                                   \
            else                                                                                                                                \
            {                                                                                                                                   \
                break;                                                                                                                          \
            }                                                                                                                                   \
        }                                                                                                                                       \
    }                                                                                                                                           \
}

#endif // LISTUTILS_H
