#ifndef LISTUTILS_H
#define LISTUTILS_H

#include "../Utils/codeutils.h"
#include "../Utils/error.h"

#define ASCENDING >
#define DESCENDING <

#define BUBBLE_SORT(condition, parameter)                                                                                                       \
{                                                                                                                                               \
    if (list != NULL && list->first != NULL && list->first->next != NULL)                                                                       \
    {                                                                                                                                           \
        ASSERT_CONDITION(list->last != NULL, "Null pointer detected for last list element")                                                     \
                                                                                                                                                \
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
                                                                                                                                                \
        /* "re-gain" the last list element after sorting */                                                                                     \
        ListElement* currentElement = list->first;                                                                                              \
        while (currentElement->next != NULL)                                                                                                    \
        {                                                                                                                                       \
            currentElement = currentElement->next;                                                                                              \
        }                                                                                                                                       \
        list->last = currentElement;                                                                                                            \
    }                                                                                                                                           \
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
    if (array != NULL && arraySize > 0)                                                                                                         \
    {                                                                                                                                           \
        BUILD_HEAP(condition, parameter)                                                                                                        \
        int currentToSortIndex = (int)arraySize-1;                                                                                              \
        while(currentToSortIndex>0)                                                                                                             \
        {                                                                                                                                       \
            swapElement(&array[0], &array[currentToSortIndex]);                                                                                 \
            --currentToSortIndex;                                                                                                               \
            int checkedElementIndex = 0;                                                                                                        \
            while (currentToSortIndex > checkedElementIndex)                                                                                    \
            {                                                                                                                                   \
                int leftChildIndex = checkedElementIndex+checkedElementIndex+1;                                                                 \
                if (currentToSortIndex > leftChildIndex)                                                                                        \
                {                                                                                                                               \
                    int rightChildIndex = leftChildIndex+1;                                                                                     \
                    int toSwapIndex = leftChildIndex;                                                                                           \
                    if (array[rightChildIndex]->parameter condition array[leftChildIndex]->parameter)                                           \
                    {                                                                                                                           \
                        toSwapIndex = rightChildIndex;                                                                                          \
                    }                                                                                                                           \
                    if (array[toSwapIndex]->parameter condition array[checkedElementIndex]->parameter)                                          \
                    {                                                                                                                           \
                        swapElement(&array[checkedElementIndex], &array[toSwapIndex]);                                                          \
                        checkedElementIndex = toSwapIndex;                                                                                      \
                    }                                                                                                                           \
                    else                                                                                                                        \
                    {                                                                                                                           \
                        break;                                                                                                                  \
                    }                                                                                                                           \
                }                                                                                                                               \
                else if (leftChildIndex == currentToSortIndex)                                                                                  \
                {                                                                                                                               \
                    if (array[leftChildIndex]->parameter condition array[checkedElementIndex]->parameter)                                       \
                    {                                                                                                                           \
                        swapElement(&array[checkedElementIndex], &array[leftChildIndex]);                                                       \
                    }                                                                                                                           \
                    break;                                                                                                                      \
                }                                                                                                                               \
                else                                                                                                                            \
                {                                                                                                                               \
                    break;                                                                                                                      \
                }                                                                                                                               \
            }                                                                                                                                   \
        }                                                                                                                                       \
    }                                                                                                                                           \
}

#define MERGE_SORT(condition, parameter, recursiveCall)                                                                                         \
{                                                                                                                                               \
    if (startIndex != endIndex)                                                                                                                 \
    {                                                                                                                                           \
        const size_t midIndex = (startIndex + endIndex) / 2;                                                                                    \
        recursiveCall(array, auxArray, startIndex, midIndex);                                                                                   \
        recursiveCall(array, auxArray, midIndex+1, endIndex);                                                                                   \
        size_t firstIndex = startIndex;                                                                                                         \
        size_t secondIndex = midIndex+1;                                                                                                        \
        size_t writeIndex = startIndex;                                                                                                         \
                                                                                                                                                \
        while (firstIndex<=midIndex && secondIndex<=endIndex)                                                                                   \
        {                                                                                                                                       \
            if (array[secondIndex]->parameter condition array[firstIndex]->parameter)                                                           \
            {                                                                                                                                   \
                auxArray[writeIndex] = array[firstIndex];                                                                                       \
                ++firstIndex;                                                                                                                   \
            }                                                                                                                                   \
            else                                                                                                                                \
            {                                                                                                                                   \
                auxArray[writeIndex] = array[secondIndex];                                                                                      \
                ++secondIndex;                                                                                                                  \
            }                                                                                                                                   \
            ++writeIndex;                                                                                                                       \
        }                                                                                                                                       \
                                                                                                                                                \
        if (firstIndex <= midIndex)                                                                                                             \
        {                                                                                                                                       \
            while (writeIndex <= endIndex)                                                                                                      \
            {                                                                                                                                   \
                auxArray[writeIndex] = array[firstIndex];                                                                                       \
                ++firstIndex;                                                                                                                   \
                ++writeIndex;                                                                                                                   \
            }                                                                                                                                   \
        }                                                                                                                                       \
        else if (secondIndex <= endIndex)                                                                                                       \
        {                                                                                                                                       \
            while (writeIndex <= endIndex)                                                                                                      \
            {                                                                                                                                   \
                auxArray[writeIndex] = array[secondIndex];                                                                                      \
                ++secondIndex;                                                                                                                  \
                ++writeIndex;                                                                                                                   \
            }                                                                                                                                   \
        }                                                                                                                                       \
                                                                                                                                                \
        for (firstIndex = startIndex; firstIndex <= endIndex; ++firstIndex)                                                                     \
        {                                                                                                                                       \
            array[firstIndex] = auxArray[firstIndex];                                                                                           \
        }                                                                                                                                       \
    }                                                                                                                                           \
}

#ifdef UNIX_OS
#define MERGE_THE_SORTED_HALF_ARRAYS(condition, parameter)                                                                                      \
{                                                                                                                                               \
    size_t firstIndex = startIndex;                                                                                                             \
    size_t secondIndex = midIndex + 1;                                                                                                          \
    size_t writeIndex = startIndex;                                                                                                             \
                                                                                                                                                \
    while (firstIndex <= midIndex && secondIndex <= endIndex)                                                                                   \
    {                                                                                                                                           \
        if (array[secondIndex]->parameter condition array[firstIndex]->parameter)                                                               \
        {                                                                                                                                       \
            auxArray[writeIndex] = array[firstIndex];                                                                                           \
            ++firstIndex;                                                                                                                       \
        }                                                                                                                                       \
        else                                                                                                                                    \
        {                                                                                                                                       \
            auxArray[writeIndex] = array[secondIndex];                                                                                          \
            ++secondIndex;                                                                                                                      \
        }                                                                                                                                       \
                                                                                                                                                \
        ++writeIndex;                                                                                                                           \
    }                                                                                                                                           \
                                                                                                                                                \
    if (firstIndex <= midIndex)                                                                                                                 \
    {                                                                                                                                           \
        while (writeIndex <= endIndex)                                                                                                          \
        {                                                                                                                                       \
            auxArray[writeIndex] = array[firstIndex];                                                                                           \
            ++firstIndex;                                                                                                                       \
            ++writeIndex;                                                                                                                       \
        }                                                                                                                                       \
    }                                                                                                                                           \
    else if (secondIndex <= endIndex)                                                                                                           \
    {                                                                                                                                           \
        while (writeIndex <= endIndex)                                                                                                          \
        {                                                                                                                                       \
            auxArray[writeIndex] = array[secondIndex];                                                                                          \
            ++secondIndex;                                                                                                                      \
            ++writeIndex;                                                                                                                       \
        }                                                                                                                                       \
    }                                                                                                                                           \
                                                                                                                                                \
    for (firstIndex=startIndex; firstIndex <= endIndex; ++firstIndex)                                                                           \
    {                                                                                                                                           \
        array[firstIndex] = auxArray[firstIndex];                                                                                               \
    }                                                                                                                                           \
}

#define ENHANCED_MERGE_SORT(condition, parameter, threadFunctionAddress)                                                                        \
{                                                                                                                                               \
    if (startIndex != endIndex)                                                                                                                 \
    {                                                                                                                                           \
        const size_t midIndex = (startIndex + endIndex)/2;                                                                                      \
        pthread_t threadSortFirstHalf, threadSortSecondHalf;                                                                                    \
        MergeSortThreadInput firstThreadInput, secondThreadInput;                                                                               \
                                                                                                                                                \
        firstThreadInput.array = array;                                                                                                         \
        firstThreadInput.auxArray = auxArray;                                                                                                   \
        firstThreadInput.startIndex = startIndex;                                                                                               \
        firstThreadInput.endIndex= midIndex;                                                                                                    \
        secondThreadInput.array = array;                                                                                                        \
        secondThreadInput.auxArray = auxArray;                                                                                                  \
        secondThreadInput.startIndex = midIndex + 1;                                                                                            \
        secondThreadInput.endIndex = endIndex;                                                                                                  \
                                                                                                                                                \
        pthread_create(&threadSortFirstHalf, NULL, threadFunctionAddress, &firstThreadInput);                                                   \
        pthread_create(&threadSortSecondHalf, NULL, threadFunctionAddress, &secondThreadInput);                                                 \
                                                                                                                                                \
        pthread_join(threadSortFirstHalf, NULL);                                                                                                \
        pthread_join(threadSortSecondHalf, NULL);                                                                                               \
                                                                                                                                                \
        MERGE_THE_SORTED_HALF_ARRAYS(condition, parameter)                                                                                      \
    }                                                                                                                                           \
}

#define QUICK_MERGE_SORT(condition, parameter, threadFunctionAddress)                                                                           \
{                                                                                                                                               \
    if (startIndex != endIndex)                                                                                                                 \
    {                                                                                                                                           \
        const size_t midIndex = (startIndex + endIndex)/2;                                                                                      \
        pthread_t threadSortFirstHalf, threadSortSecondHalf;                                                                                    \
        QuickSortThreadInput firstThreadInput, secondThreadInput;                                                                               \
                                                                                                                                                \
        firstThreadInput.array = array;                                                                                                         \
        firstThreadInput.startIndex = startIndex;                                                                                               \
        firstThreadInput.endIndex = midIndex;                                                                                                   \
        secondThreadInput.array = array;                                                                                                        \
        secondThreadInput.startIndex = midIndex + 1;                                                                                            \
        secondThreadInput.endIndex = endIndex;                                                                                                  \
                                                                                                                                                \
        pthread_create(&threadSortFirstHalf, NULL, threadFunctionAddress, &firstThreadInput);                                                   \
        pthread_create(&threadSortSecondHalf, NULL, threadFunctionAddress, &secondThreadInput);                                                 \
                                                                                                                                                \
        pthread_join(threadSortFirstHalf, NULL);                                                                                                \
        pthread_join(threadSortSecondHalf, NULL);                                                                                               \
                                                                                                                                                \
        MERGE_THE_SORTED_HALF_ARRAYS(condition, parameter)                                                                                      \
    }                                                                                                                                           \
}
#endif

#define QUICK_SORT(condition, parameter, recursiveCall)                                                                                         \
{                                                                                                                                               \
    if (beginIndex != endIndex)                                                                                                                 \
    {                                                                                                                                           \
        if (beginIndex == endIndex-1)                                                                                                           \
        {                                                                                                                                       \
            if (array[beginIndex]->parameter condition array[endIndex]->parameter)                                                              \
            {                                                                                                                                   \
                swapElement(&array[beginIndex], &array[endIndex]);                                                                              \
            }                                                                                                                                   \
        }                                                                                                                                       \
        else                                                                                                                                    \
        {                                                                                                                                       \
            const ListElement* pivot = array[beginIndex];                                                                                       \
            size_t leftIndex = beginIndex+1;                                                                                                    \
            size_t rightIndex = endIndex;                                                                                                       \
                                                                                                                                                \
            while (leftIndex <= rightIndex)                                                                                                     \
            {                                                                                                                                   \
                while ((pivot->parameter condition array[leftIndex]->parameter || pivot->parameter == array[leftIndex]->parameter) &&           \
                                                                                                            leftIndex < endIndex)               \
                {                                                                                                                               \
                    ++leftIndex;                                                                                                                \
                }                                                                                                                               \
                while (array[rightIndex]->parameter condition pivot->parameter && rightIndex > beginIndex)                                      \
                {                                                                                                                               \
                    --rightIndex;                                                                                                               \
                }                                                                                                                               \
                if (leftIndex < rightIndex)                                                                                                     \
                {                                                                                                                               \
                    swapElement(&array[leftIndex], &array[rightIndex]);                                                                         \
                    ++leftIndex;                                                                                                                \
                    --rightIndex;                                                                                                               \
                }                                                                                                                               \
                else                                                                                                                            \
                {                                                                                                                               \
                    break;                                                                                                                      \
                }                                                                                                                               \
            }                                                                                                                                   \
            if (rightIndex > beginIndex)                                                                                                        \
            {                                                                                                                                   \
                swapElement(&array[beginIndex], &array[rightIndex]);                                                                            \
                recursiveCall(array,beginIndex,rightIndex-1);                                                                                   \
            }                                                                                                                                   \
            if (rightIndex < endIndex)                                                                                                          \
            {                                                                                                                                   \
                recursiveCall(array,rightIndex+1,endIndex);                                                                                     \
            }                                                                                                                                   \
        }                                                                                                                                       \
    }                                                                                                                                           \
}

#ifdef UNIX_OS
#define ENHANCED_QUICK_SORT(condition, parameter, threadFunctionAddress, recursiveCall)                                                         \
{                                                                                                                                               \
    if (beginIndex != endIndex)                                                                                                                 \
    {                                                                                                                                           \
        if (beginIndex == endIndex-1)                                                                                                           \
        {                                                                                                                                       \
            if (array[beginIndex] > array[endIndex])                                                                                            \
            {                                                                                                                                   \
                swapElement(&array[beginIndex], &array[endIndex]);                                                                              \
            }                                                                                                                                   \
        }                                                                                                                                       \
        else                                                                                                                                    \
        {                                                                                                                                       \
            const ListElement* pivot = array[beginIndex];                                                                                       \
            size_t leftIndex = beginIndex+1;                                                                                                    \
            size_t rightIndex = endIndex;                                                                                                       \
                                                                                                                                                \
            while (leftIndex <= rightIndex)                                                                                                     \
            {                                                                                                                                   \
                while ((pivot->parameter condition array[leftIndex]->parameter || pivot->parameter == array[leftIndex]->parameter) &&           \
                        leftIndex < endIndex)                                                                                                   \
                {                                                                                                                               \
                    ++leftIndex;                                                                                                                \
                }                                                                                                                               \
                while (array[rightIndex]->parameter condition pivot->parameter && rightIndex > beginIndex)                                      \
                {                                                                                                                               \
                    --rightIndex;                                                                                                               \
                }                                                                                                                               \
                if (leftIndex < rightIndex)                                                                                                     \
                {                                                                                                                               \
                    swapElement(&array[leftIndex], &array[rightIndex]);                                                                         \
                    ++leftIndex;                                                                                                                \
                    --rightIndex;                                                                                                               \
                }                                                                                                                               \
                else                                                                                                                            \
                {                                                                                                                               \
                    break;                                                                                                                      \
                }                                                                                                                               \
            }                                                                                                                                   \
                                                                                                                                                \
            if (rightIndex > beginIndex)                                                                                                        \
            {                                                                                                                                   \
                swapElement(&array[beginIndex], &array[rightIndex]);                                                                            \
                                                                                                                                                \
                pthread_t threadSortFirstHalf;                                                                                                  \
                QuickSortThreadInput firstThreadInput;                                                                                          \
                firstThreadInput.array = array;                                                                                                 \
                firstThreadInput.startIndex = beginIndex;                                                                                       \
                firstThreadInput.endIndex = rightIndex - 1;                                                                                     \
                pthread_create(&threadSortFirstHalf, NULL, threadFunctionAddress, &firstThreadInput);                                           \
                                                                                                                                                \
                if (rightIndex < endIndex)                                                                                                      \
                {                                                                                                                               \
                    pthread_t threadSortSecondHalf;                                                                                             \
                    QuickSortThreadInput secondThreadInput;                                                                                     \
                    secondThreadInput.array = array;                                                                                            \
                    secondThreadInput.startIndex = rightIndex + 1;                                                                              \
                    secondThreadInput.endIndex = endIndex;                                                                                      \
                    pthread_create(&threadSortSecondHalf, NULL, threadFunctionAddress, &secondThreadInput);                                     \
                    pthread_join(threadSortSecondHalf, NULL);                                                                                   \
                }                                                                                                                               \
                                                                                                                                                \
                pthread_join(threadSortFirstHalf, NULL);                                                                                        \
            }                                                                                                                                   \
            else if (rightIndex < endIndex)                                                                                                     \
            {                                                                                                                                   \
                recursiveCall(array, rightIndex+1, endIndex);                                                                                   \
            }                                                                                                                                   \
        }                                                                                                                                       \
    }                                                                                                                                           \
}
#endif

#define CHECK_IF_SORTED(condition, parameter)                                                                                                   \
{                                                                                                                                               \
    bool isSorted = true;                                                                                                                    \
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
        isSorted = false;                                                                                                                       \
    }                                                                                                                                           \
                                                                                                                                                \
    return isSorted;                                                                                                                            \
}

#endif // LISTUTILS_H
