#include <iostream>
#include <thread>

#include "asyncutils.h"
#include "codeutils.h"

int main()
{
    size_t nrOfElements{requestListElementsCountFromUser()};
    clearScreen();

    std::vector<size_t> receivedPriorities{};
    if (nrOfElements > 0)
    {
        std::cout << "Launching async task...";
        std::future<std::vector<size_t>> result{std::async(std::launch::async, generateListElementPriorities, nrOfElements)};
        std::cout << "DONE" << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds{500});
        std::cout << "Waiting for result..." << std::endl << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds{500});

        getDataFromTask(result, receivedPriorities);

        if (receivedPriorities.size() > 0)
        {
            putPrioritiesIntoList(receivedPriorities);
        }
        else
        {
            std::cout << "The list cannot be created" << std::endl;
        }
    }
    else
    {
        std::cout<<"You exited the app!"<<std::endl;
    }

    return 0;
}
