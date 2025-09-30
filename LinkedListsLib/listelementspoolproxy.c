#include <stdio.h>

#include "listelementspoolproxy.h"
#include "listelementspool.h"
#include "error.h"

ListElement* aquireListElement(ListElementsPoolProxy* elementsPoolProxy)
{
    ListElement* element = NULL;
    ListElementsPool* elementsPool = elementsPoolProxy != NULL ? (ListElementsPool*)elementsPoolProxy->elementsPool : NULL;

    ASSERT(elementsPoolProxy == NULL || elementsPool != NULL, "Invalid list elements pool proxy!");

    if (elementsPool != NULL)
    {
        element = aquireElement(elementsPool);
    }

    return element;
}

bool aquireListElements(ListElementsPoolProxy* elementsPoolProxy, ListElement** elements, size_t requiredElementsCount)
{
    bool result = false;
    ListElementsPool* elementsPool = elementsPoolProxy != NULL ? (ListElementsPool*)elementsPoolProxy->elementsPool : NULL;

    ASSERT(elementsPoolProxy == NULL || elementsPool != NULL, "Invalid list elements pool proxy!");

    if (elementsPool != NULL)
    {
        result = aquireElements(elementsPool, elements, requiredElementsCount);
    }

    return result;
}

bool releaseListElement(ListElement* element, ListElementsPoolProxy* elementsPoolProxy)
{
    bool result = false;
    ListElementsPool* elementsPool = elementsPoolProxy != NULL ? (ListElementsPool*)elementsPoolProxy->elementsPool : NULL;

    ASSERT(elementsPoolProxy == NULL || elementsPool != NULL, "Invalid list elements pool proxy!");

    if (elementsPool != NULL)
    {
        result = releaseElement(element, elementsPool);
    }

    return result;
}
