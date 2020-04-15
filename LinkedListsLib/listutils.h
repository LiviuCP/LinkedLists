#ifndef LISTMACROS_H
#define LISTMACROS_H

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

#endif // LISTMACROS_H
