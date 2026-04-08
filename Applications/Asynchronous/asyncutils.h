#pragma once

#include <future>
#include <vector>

#include "listelement.h"

size_t requestListElementsCountFromUser();
std::vector<Priority> generateListElementPriorities(size_t nrOfElements);
void getDataFromTask(std::future<std::vector<Priority>>& result, std::vector<Priority>& data);
void putPrioritiesIntoList(const std::vector<Priority>& receivedPriorities);
