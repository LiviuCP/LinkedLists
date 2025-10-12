#include <stdio.h>
#include <stdlib.h>

#include "listprintutils.h"
#include "codeutils.h"

int main()
{
    size_t priority = 0;
    size_t index = 0;
    List* list = createEmptyList(NULL);

    if (list != NULL)
    {
        printf("Let's enter the elements (enter 0 to finish)!\n");
        for (;;)
        {
            printf("Element priority: ");
            if (!readUnsignedLong(&priority))
            {
                clearScreen();
                printf("Invalid input! Please try again\n\n");
                continue;
            }

            if (priority > 0)
            {
                createAndAppendToList(list, priority);
            }
            else
            {
                break;
            }
        }


        clearScreen();
        printf("You created the list. Now you can choose one of the below options.\n\n");

        for (;;)
        {

            printf("List content: \n\n");
            printList(list);
            printListToFile(list);
            printf("\nEnter the required option:\n");
            printf("1 - append element\n");
            printf("2 - prepend element\n");
            printf("3 - remove first element\n");
            printf("4 - remove last element\n");
            printf("5 - sort ascending by priority\n");
            printf("6 - reverse list elements\n");
            printf("7 - empty list\n");
            printf("8 - get element from chosen index\n");
            printf("0 - exit\n");

            size_t choice;
            if (!readUnsignedLong(&choice))
            {
                clearScreen();
                printf("Invalid input! Please try again\n\n");
                continue;
            }
            else
            {
                switch (choice)
                {
                case 0:
                    // erase list
                    deleteList(list, deleteObjectPayload);
                    clearScreen();
                    printf("You exited the app!\n");
                    break;
                case 1:
                    printf("\nEnter the element priority: ");
                    if (!readUnsignedLong(&priority))
                    {
                        clearScreen();
                        printf("Invalid input. No element appended to list\n\n");
                        continue;
                    }
                    else if (priority == 0)
                    {
                        clearScreen();
                        printf("Priority should be greater than 0. No element appended to list\n\n");
                        continue;
                    }
                    else
                    {
                        createAndAppendToList(list, priority);
                        clearScreen();
                        printf("Element appended\n\n");
                    }
                    break;
                case 2:
                    printf("\nEnter the priority: ");
                    if (!readUnsignedLong(&priority))
                    {
                        clearScreen();
                        printf("Invalid input. No element prepended to list\n\n");
                        continue;
                    }
                    else if (priority == 0)
                    {
                        clearScreen();
                        printf("Priority should be greater than 0. No element prepended to list\n\n");
                        continue;
                    }
                    else
                    {
                        createAndPrependToList(list, priority);
                        clearScreen();
                        printf("Element prepended\n\n");
                    }
                    break;
                case 3:
                    if (!isEmptyList(list))
                    {
                        removeFirstListElement(list);
                        clearScreen();
                        printf("First element removed\n\n");
                    }
                    else
                    {
                        clearScreen();
                        printf("Cannot remove first element! The list is empty.\n\n");
                    }
                    break;
                case 4:
                    if (!isEmptyList(list))
                    {
                        removeLastListElement(list);
                        clearScreen();
                        printf("Last element removed\n\n");
                    }
                    else
                    {
                        clearScreen();
                        printf("Cannot remove last element! The list is empty.\n\n");
                    }
                    break;
                case 5:
                    if (!isEmptyList(list))
                    {
                        sortAscendingByPriority(list);
                        clearScreen();
                        printf("The list has been sorted.\n\n");
                    }
                    else
                    {
                        clearScreen();
                        printf("Nothing to sort, list is empty\n\n");
                    }
                    break;
                case 6:
                    if (!isEmptyList(list))
                    {
                        reverseList(list);
                        clearScreen();
                        printf("The list has been reversed.\n\n");
                    }
                    else
                    {
                        clearScreen();
                        printf("Nothing to reverse, list is empty\n\n");
                    }
                    break;
                case 7:
                    if (!isEmptyList(list))
                    {
                        clearList(list, deleteObjectPayload);
                        clearScreen();
                        printf("The list has been emptied\n\n");
                    }
                    else
                    {
                        clearScreen();
                        printf("The list is already empty!\n\n");
                    }
                    break;
                case 8:
                    printf("\nEnter the index: ");
                    if (!readUnsignedLong(&index))
                    {
                        clearScreen();
                        printf("Invalid input. No element appended to list\n\n");
                        continue;
                    }
                    else if (index > getListSize(list))
                    {
                        clearScreen();
                        printf("Invalid index (out of bounds)!\n\n");
                        continue;
                    }
                    else
                    {
                        clearScreen();
                        printf("Element with index %d has priority %zu\n\n", (unsigned int)index, (size_t)getListElementAtIndex(list, index)->priority);
                    }
                    break;
                default:
                    clearScreen();
                    printf("Invalid choice\n\n");
                }
            }

            if (choice == 0)
            {
                break;
            }
        }
    }
    else
    {
        printf("Unable to create list: no memory allocated\n");
        exit(-1);
    }

    return 0;
}
