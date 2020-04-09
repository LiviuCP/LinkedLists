#include <stdio.h>

#include "linkedlist.h"

int main()
{
    int currentId = 0;
    List* list = (List*)malloc(sizeof(List));
    printf("Lets enter the elements (enter 0 to exit)!\n");
    do
    {
        printf("Element Id: ");
        scanf("%d", &currentId);
        if (currentId > 0)
        {
            ListElement* element = (ListElement*)malloc(sizeof(ListElement));
            element->id = currentId;
            appendElement(list, element);
        }
    }
    while (currentId > 0);

    int choice = 0;

    do
    {
        system("clear");
        printf("List content: \n\n");
        printList(list);
        printf("Enter the required option:\n");
        printf("1 - append element\n");
        printf("2 - prepend element\n");
        printf("3 - remove first element\n");
        printf("4 - remove last element\n");
        printf("0 - exit\n");

        scanf("%d", &choice);

        switch (choice)
        {
        case 0:
            // erase list
            if (list->first != NULL)
            {
                free(removeFirstElement(list));
            }
            free(list);
            printf("You exited the app!\n");
            break;
        case 1:
            printf("\nEnter the id: ");
            scanf("%d", &currentId);
        {
            ListElement* element = (ListElement*)malloc(sizeof(ListElement));
            element->id = currentId;
            appendElement(list, element);
        }
            break;
        case 2:
            printf("\nEnter the id: ");
            scanf("%d", &currentId);
        {
            ListElement* element = (ListElement*)malloc(sizeof(ListElement));
            element->id = currentId;
            prependElement(list, element);
        }
            break;
        case 3:
            removeFirstElement(list);
            break;
        case 4:
            removeLastElement(list);
            break;
        default:
            printf("Invalid option\n");
        }
        if (choice > 0)
        {
            printf("List content: \n\n");
            printList(list);
        }
    }
    while (choice > 0);

    return 0;
}
