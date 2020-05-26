#include <iostream>
#include <vector>
#include <array>
#include <thread>
#include <future>

#include "../../LinkedListsLib/linkedlist.h"
#include "../../Utils/codeutils.h"
#include "../ManualListEntry/apputils.h"

std::vector<size_t> generateListElementPriorities(size_t nrOfElements);
size_t requestInputFromUser();

int main()
{
    using namespace std;

    size_t nrOfElements{requestInputFromUser()};

    std::vector<size_t> receivedPriorities{};
    if (nrOfElements > 0)
    {
        cout << "Launching async task...";
        std::future<std::vector<size_t>> result = std::async(std::launch::async, generateListElementPriorities, nrOfElements);
        cout << "DONE" << endl;
        std::this_thread::sleep_for(std::chrono::milliseconds{500});
        cout << "Waiting for result..." << endl << endl;
        std::this_thread::sleep_for(std::chrono::milliseconds{500});

        while(result.valid())
        {
            if (result.wait_for(std::chrono::seconds{1}) == std::future_status::ready)
            {
                receivedPriorities = result.get();
                std::this_thread::sleep_for(std::chrono::milliseconds{500});
                cout << "The wait is over! Received priorities for " << receivedPriorities.size() << " elements" << endl << endl;
            }
            else
            {
                cout << "The result is not yet available. Going back to sleep..." << endl;
            }
        }

        if (receivedPriorities.size() > 0)
        {
            std::this_thread::sleep_for(std::chrono::milliseconds{500});
            cout << "Constructing and sorting list descending...";
            std::this_thread::sleep_for(std::chrono::seconds{1});
            List* list{createListFromPrioritiesArray(&receivedPriorities[0], receivedPriorities.size())};
            sortDescendingByPriority(list);
            cout << "DONE" << endl;
            std::this_thread::sleep_for(std::chrono::seconds{2});
            cout << endl << "The sorted list has following content:" << endl << endl;
            printList(list);
            cout << endl;
            deleteList(list, deleteObjectPayload);
            list = nullptr;
        }
        else
        {
            cout << "The list cannot be created" << endl;
        }
    }
    else
    {
        cout<<"You exited the app!"<<endl;
    }

    return 0;
}

std::vector<size_t> generateListElementPriorities(size_t nrOfElements)
{
    using namespace std;
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

size_t requestInputFromUser()
{
    using namespace std;

    size_t result = 0;

    while(true)
    {
        cout << "Enter the number of elements (enter 0 to quit): ";

        if (!readUnsignedLong(&result))
        {
            cout << "Invalid Value! Please try again" << endl << endl;
        }
        else
        {
            break;
        }
    }

    return result;
}
