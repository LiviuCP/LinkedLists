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
                    ListElement* swap = array[checkedIndex];                                                                                    \
                    array[checkedIndex] = array[elementToInsert];                                                                               \
                    array[elementToInsert] = swap;                                                                                              \
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

#endif // LISTUTILS_H
