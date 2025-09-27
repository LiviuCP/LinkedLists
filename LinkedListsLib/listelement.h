#ifndef LISTELEMENT_H
#define LISTELEMENT_H

#include "codeutils.h"

#define ELEMENTS_POOL_SLICE_SIZE 128

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

typedef struct
{
    void* poolContent;
}
ListElementsPool;

// don't use for stack created lists (heap-only)
ListElement* createListElement();
void initListElement(ListElement* element);

ListElementsPool* createListElementsPool();
void deleteListElementsPool(ListElementsPool* elementsPool);
size_t getAvailableElementsCount(ListElementsPool* elementsPool);
ListElement* aquireElement(ListElementsPool* elementsPool);
bool aquireElements(ListElementsPool* elementsPool, ListElement** elements, size_t requiredElementsCount);
bool releaseElement(ListElement* element, ListElementsPool* elementsPool);
void shrinkPoolCapacity(ListElementsPool* elementsPool);

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
