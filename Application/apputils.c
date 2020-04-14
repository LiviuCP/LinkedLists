#include <stdio.h>
#include <stdlib.h>
#include "apputils.h"

void printList(List* list)
{
    if (list != NULL)
    {
        int currentElementIndex = 0;
        for (ListIterator it = lbegin(list); !areIteratorsEqual(it, lend(list)); lnext(&it))
        {
            printf("Index: %d\t Priority: %lu\n", currentElementIndex+1, it.current->priority);
            ++currentElementIndex;
        }
        if (currentElementIndex == 0)
        {
            printf("Empty list!\n\n");
        }
        else
        {
            printf("\nThe list has %d elements\n\n", (int)getListSize(list));
        }
    }
    else
    {
        printf("The list does not exist!");
    }
}
