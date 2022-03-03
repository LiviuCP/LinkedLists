#ifndef ASYNCUTILS_H
#define ASYNCUTILS_H

#include <vector>
#include <array>
#include <future>

size_t requestListElementsCountFromUser();
std::vector<size_t> generateListElementPriorities(size_t nrOfElements);
void getDataFromTask(std::future<std::vector<size_t>>& result, std::vector<size_t>& data);
void putPrioritiesIntoList(const std::vector<size_t> receivedPriorities);

#endif // ASYNCUTILS_H
