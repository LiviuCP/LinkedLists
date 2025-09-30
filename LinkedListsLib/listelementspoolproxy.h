#ifndef LISTELEMENTSPOOLPROXY_H
#define LISTELEMENTSPOOLPROXY_H

#include "listelement.h"

/* The proxy is used by consumers (typically link lists) to connect to the elements pool in order to aquire/release elements
    - provides decoupling: it is possible to use multiple list elements pool types to provide elements to the consumers
    - prevents consumers from accessing pool functionality they are not supposed to access, e.g. optimizing the pool capacity
*/

typedef struct
{
    void* elementsPool;
} ListElementsPoolProxy;

#ifdef __cplusplus
extern "C"{
#endif

ListElement* aquireListElement(ListElementsPoolProxy* elementsPoolProxy);
bool aquireListElements(ListElementsPoolProxy* elementsPoolProxy, ListElement** elements, size_t requiredElementsCount);
bool releaseListElement(ListElement* element, ListElementsPoolProxy* elementsPoolProxy);

#ifdef __cplusplus
}
#endif

#endif // LISTELEMENTSPOOLPROXY_H
