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
            printf("Element number: %d\t Priority: %lu\n", currentElementIndex+1, it.current->priority);
            ++currentElementIndex;
        }
        if (currentElementIndex == 0)
        {
            printf("Empty list!\n\n");
        }
        else
        {
            printf("\nThe list has %d elements\n", (int)getListSize(list));
        }
    }
    else
    {
        printf("The list does not exist!");
    }
}

void printListToFile(List* list)
{
    const char* outputFile = "/tmp/test.txt";
    const char* header = "The list has following content:\n\n";

    printListContentToFile(list, outputFile, header);
}
