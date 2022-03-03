#ifndef APPUTILS_H
#define APPUTILS_H

#include "linkedlist.h"

#ifdef __cplusplus
extern "C"{
#endif

void printList(List* list);
void printListToFile(List* list);

#ifdef __cplusplus
}
#endif

#endif // APPUTILS_H
