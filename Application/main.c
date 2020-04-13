#include <stdio.h>
#include <stdlib.h>

#include "../LinkedListsLib/linkedlist.h"
#include "../Utils/codeutils.h"


int main()
{
    size_t currentElementPriority = 0;
    List* list = (List*)malloc(sizeof(List));
    printf("Let's enter the elements (enter 0 to finish)!\n");
    for (;;)
    {
        printf("Element priority: ");
        if (!readUnsignedLong(&currentElementPriority))
        {
            system("clear");
            printf("Invalid input! Please try again\n\n");
            continue;
        }

        if (currentElementPriority > 0)
        {
            ListElement* element = (ListElement*)malloc(sizeof(ListElement));
            element->priority = currentElementPriority;
            appendElementToList(list, element);
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
        printf("Enter the required option:\n");
        printf("1 - append element\n");
        printf("2 - prepend element\n");
        printf("3 - remove first element\n");
        printf("4 - remove last element\n");
        printf("5 - sort ascending by priority\n");
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
                if (list->first != NULL)
                {
                    free(removeFirstListElement(list));
                }
                free(list);
                system("clear");
                printf("You exited the app!\n");
                break;
            case 1:
                printf("\nEnter the element priority: ");
                if (!readUnsignedLong(&currentElementPriority))
                {
                    system("clear");
                    printf("Invalid input. No element appended to list\n\n");
                    continue;
                }
                else if (currentElementPriority == 0)
                {
                    system("clear");
                    printf("Priority should be greater than 0. No element appended to list\n\n");
                    continue;
                }
                else
                {
                    ListElement* element = (ListElement*)malloc(sizeof(ListElement));
                    element->priority = currentElementPriority;
                    appendElementToList(list, element);
                    system("clear");
                    printf("Element appended\n\n");
                }
                break;
            case 2:
                printf("\nEnter the priority: ");
                if (!readUnsignedLong(&currentElementPriority))
                {
                    system("clear");
                    printf("Invalid input. No element prepended to list\n\n");
                    continue;
                }
                else if (currentElementPriority == 0)
                {
                    system("clear");
                    printf("Priority should be greater than 0. No element prepended to list\n\n");
                    continue;
                }
                else
                {
                    ListElement* element = (ListElement*)malloc(sizeof(ListElement));
                    element->priority = currentElementPriority;
                    prependElementToList(list, element);
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
