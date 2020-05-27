#include <iostream>
#include <thread>

#include "asyncutils.h"
#include "../../LinkedListsLib/linkedlist.h"
#include "../../Utils/codeutils.h"
#include "../ManualListEntry/apputils.h"

size_t requestInputFromUser()
{
    size_t result = 0;

    while(true)
    {
        std::cout << "Enter the number of elements (enter 0 to quit): ";

        if (!readUnsignedLong(&result))
        {
            std::cout << "Invalid Value! Please try again" << std::endl << std::endl;
        }
        else
        {
            break;
        }
    }

    return result;
}

std::vector<size_t> generateListElementPriorities(size_t nrOfElements)
{
    static const std::array<size_t, 12> c_AvailablePriorities{2, 1, 5, 8, 14, 4, 6, 12, 11, 7, 5, 18};
    std::vector<size_t> result{};

    if (nrOfElements > 0)
    {
        const size_t providedNrOfPriorities{nrOfElements <= c_AvailablePriorities.size() ? nrOfElements : c_AvailablePriorities.size()};

        for (size_t index{0}; index < providedNrOfPriorities; ++index)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds{500});
            result.push_back(c_AvailablePriorities[index]);
        }
    }

    return result;
}

void getDataFromTask(std::future<std::vector<size_t> > &result, std::vector<size_t> &data)
{
    while(result.valid())
    {
        if (result.wait_for(std::chrono::seconds{1}) == std::future_status::ready)
        {
            data = result.get();
            std::this_thread::sleep_for(std::chrono::milliseconds{500});
            std::cout << "The wait is over! Received priorities for " << data.size() << " elements" << std::endl << std::endl;
        }
        else
        {
            std::cout << "The result is not yet available. Going back to sleep..." << std::endl;
        }
    }
}

void putPrioritiesIntoList(const std::vector<size_t> receivedPriorities)
{
    std::this_thread::sleep_for(std::chrono::milliseconds{500});
    std::cout << "Constructing list and sorting descending...";
    std::this_thread::sleep_for(std::chrono::seconds{1});
    List* list{createListFromPrioritiesArray(&receivedPriorities[0], receivedPriorities.size())};
    sortDescendingByPriority(list);
    std::cout << "DONE" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds{2});
    std::cout << std::endl << "The sorted list has following content:" << std::endl << std::endl;
    printList(list);
    std::cout << std::endl;
    deleteList(list, deleteObjectPayload);
    list = nullptr;
}
