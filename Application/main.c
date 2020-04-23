#include <stdio.h>
#include <stdlib.h>

#include "apputils.h"
#include "../LinkedListsLib/linkedlist.h"
#include "../Utils/codeutils.h"

int main()
{
    size_t priority = 0;
    size_t index = 0;
    List* list = (List*)malloc(sizeof(List));
    printf("Let's enter the elements (enter 0 to finish)!\n");
    for (;;)
    {
        printf("Element priority: ");
        if (!readUnsignedLong(&priority))
        {
            system("clear");
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


    system("clear");
    printf("You created the list. Now you can choose one of the below options.\n\n");

    for (;;)
    {

        printf("List content: \n\n");
        printList(list);
        printListToFile(list);
        printf("Enter the required option:\n");
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
            system("clear");
            printf("Invalid input! Please try again\n\n");
            continue;
        }
        else
        {
            switch (choice)
            {
            case 0:
                // erase list
                deleteList(list, deleteObject);
                system("clear");
                printf("You exited the app!\n");
                break;
            case 1:
                printf("\nEnter the element priority: ");
                if (!readUnsignedLong(&priority))
                {
                    system("clear");
                    printf("Invalid input. No element appended to list\n\n");
                    continue;
                }
                else if (priority == 0)
                {
                    system("clear");
                    printf("Priority should be greater than 0. No element appended to list\n\n");
                    continue;
                }
                else
                {
                    createAndAppendToList(list, priority);
                    system("clear");
                    printf("Element appended\n\n");
                }
                break;
            case 2:
                printf("\nEnter the priority: ");
                if (!readUnsignedLong(&priority))
                {
                    system("clear");
                    printf("Invalid input. No element prepended to list\n\n");
                    continue;
                }
                else if (priority == 0)
                {
                    system("clear");
                    printf("Priority should be greater than 0. No element prepended to list\n\n");
                    continue;
                }
                else
                {
                    createAndPrependToList(list, priority);
                    system("clear");
                    printf("Element prepended\n\n");
                }
                break;
            case 3:
                if (getListSize(list) != 0)
                {
                    removeFirstListElement(list);
                    system("clear");
                    printf("First element removed\n\n");
                }
                else
                {
                    system("clear");
                    printf("Cannot remove first element! The list is empty.\n\n");
                }
                break;
            case 4:
                if (getListSize(list) != 0)
                {
                    removeLastListElement(list);
                    system("clear");
                    printf("Last element removed\n\n");
                }
                else
                {
                    system("clear");
                    printf("Cannot remove last element! The list is empty.\n\n");
                }
                break;
            case 5:
                if (getListSize(list) != 0)
                {
                    sortAscendingByPriority(list);
                    system("clear");
                    printf("The list has been sorted.\n\n");
                }
                else
                {
                    system("clear");
                    printf("Nothing to sort, list is empty\n\n");
                }
                break;
            case 6:
                if (getListSize(list) != 0)
                {
                    reverseList(list);
                    system("clear");
                    printf("The list has been reversed.\n\n");
                }
                else
                {
                    system("clear");
                    printf("Nothing to reverse, list is empty\n\n");
                }
                break;
            case 7:
                if (getListSize(list) != 0)
                {
                    clearList(list, deleteObject);
                    system("clear");
                    printf("The list has been emptied\n\n");
                }
                else
                {
                    system("clear");
                    printf("The list is already empty!\n\n");
                }
                break;
            case 8:
                printf("\nEnter the index: ");
                if (!readUnsignedLong(&index))
                {
                    system("clear");
                    printf("Invalid input. No element appended to list\n\n");
                    continue;
                }
                else if (index > getListSize(list))
                {
                    system("clear");
                    printf("Invalid index (out of bounds)!\n\n");
                    continue;
                }
                else
                {
                    system("clear");
                    printf("Element with index %d has priority %zu\n\n", (unsigned int)index, (size_t)getElementAtIndex(list, index)->priority);
                }
                break;
            default:
                system("clear");
                printf("Invalid choice\n\n");
            }
        }

        if (choice == 0)
        {
            break;
        }
    }

    return 0;
}
