#ifndef LISTELEMENT_H
#define LISTELEMENT_H

#include "codeutils.h"

#define MAX_POOLED_ELEMENTS_COUNT 1024

#ifdef __cplusplus
extern "C"{
#endif

struct ListElement
{
    Object object;
    size_t priority;

    struct ListElement* next;
};

typedef struct ListElement ListElement;

// TODO: move pool to separate file (along with ListElement)
typedef struct
{
    void* content;
}
ListElementsPool;

// don't use for stack created lists (heap-only)
ListElement* createListElement();

ListElementsPool* createListElementsPool();
void deleteListElementsPool(ListElementsPool* elementsPool);
size_t getAvailableElementsCount(ListElementsPool* elementsPool);
ListElement* aquireElement(ListElementsPool* elementsPool);
bool releaseElement(ListElement* element, ListElementsPool* elementsPool);

void assignObjectContentToListElement(ListElement* element, const int objectType, void* const objectPayload);
Object* detachContentFromListElement(ListElement* element);

/* don't use these two for stack created lists (heap-only) */
void deleteObjectPayload(Object* object); // default object deallocator, only works for simple objects without associated payload heap memory (e.g. Point, primitive types payloads)
bool copyObjectPlaceholder(const ListElement* source, ListElement* destination); // default object copy function, does nothing but is required for passing a default function pointer

// for testing purposes only
bool customCopyObject(const ListElement* source, ListElement* destination);

#ifdef __cplusplus
}
#endif

#endif // LISTELEMENT_H
