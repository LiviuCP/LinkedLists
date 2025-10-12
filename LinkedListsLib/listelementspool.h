#ifndef LISTELEMENTSPOOL_H
#define LISTELEMENTSPOOL_H

#include "listelement.h"

#define ELEMENTS_POOL_SLICE_SIZE 128

/* The list elements pool consists of a fixed number of slices, each slice having a fixed number of elements that can be aquired for usage
   - when the pool gets created only the first slice is available
   - when the slice is fully allocated a new slice is added; only one slice can be added at a time
   - this continues until all slices have been created
   - when the last slice got filled in, no more elements can be aquired until at least one element gets released
   - the maximum number of elements that can be aquired in the same time cannot exceed the elements count of two slices
   - unused slices can be deleted by running the shrinkPoolCapacity() function
   - deleting slices should abide to the condition that the number of available elements after deletion needs to be greater than 0; at least one slice (partially occupied or free) should exist
   - currently there is a fixed maximum number of slices
   - the list elements pool should not be used for lists created on the stack
*/

typedef struct
{
    void* poolContent;
}
ListElementsPool;

#ifdef __cplusplus
extern "C"{
#endif

ListElementsPool* createListElementsPool(size_t maxSlicesCount);
void deleteListElementsPool(ListElementsPool* elementsPool);
ListElement* aquireElement(ListElementsPool* elementsPool);
bool aquireElements(ListElementsPool* elementsPool, ListElement** elements, size_t requiredElementsCount);
bool releaseElement(ListElement* element, ListElementsPool* elementsPool);
void shrinkPoolCapacity(ListElementsPool* elementsPool);
size_t getAvailableElementsCount(ListElementsPool* elementsPool);
size_t getAquiredElementsCount(ListElementsPool* elementsPool);

#ifdef __cplusplus
}
#endif

#endif // LISTELEMENTSPOOL_H
