#ifndef ASYNCUTILS_H
#define ASYNCUTILS_H

#include <vector>
#include <future>

#include "listelement.h"

size_t requestListElementsCountFromUser();
std::vector<Priority> generateListElementPriorities(size_t nrOfElements);
void getDataFromTask(std::future<std::vector<Priority>>& result, std::vector<Priority>& data);
void putPrioritiesIntoList(const std::vector<Priority>& receivedPriorities);

#endif // ASYNCUTILS_H
