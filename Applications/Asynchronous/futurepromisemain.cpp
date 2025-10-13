#include <iostream>
#include <thread>
#include <future>

#include "asyncutils.h"
#include "codeutils.h"

void wrapperGeneratePriorities(size_t nrOfElements, std::promise<std::vector<Priority>> prioritiesRetrievalPromise);  // move semantics
//void wrapperGeneratePriorities(size_t nrOfElements, std::promise<std::vector<Priority>>* prioritiesRetrievalPromise); // pointer
//void wrapperGeneratePriorities(size_t nrOfElements, std::promise<std::vector<Priority>>& prioritiesRetrievalPromise); // reference

int main()
{
    size_t nrOfElements{requestListElementsCountFromUser()};
    clearScreen();

    std::vector<Priority> receivedPriorities{};
    if (nrOfElements > 0)
    {
        std::cout << "Launching thread...";
        std::promise<std::vector<Priority>> resultPromise;
        std::future<std::vector<Priority>> result{resultPromise.get_future()};
        std::thread prioritiesRetrieval{wrapperGeneratePriorities, nrOfElements, std::move(resultPromise)}; // move semantics
//        std::thread prioritiesRetrieval{wrapperGeneratePriorities, nrOfElements, &resultPromise}; // pointer
//        std::thread prioritiesRetrieval{wrapperGeneratePriorities, nrOfElements, std::ref(resultPromise)}; // reference
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

        std::this_thread::sleep_for(std::chrono::milliseconds{500});
        std::cout << "Joining thread...";
        std::this_thread::sleep_for(std::chrono::milliseconds{500});
        prioritiesRetrieval.join();
        std::cout << "DONE" << std::endl;
    }
    else
    {
        std::cout<<"You exited the app!"<<std::endl;
    }

    return 0;
}

void wrapperGeneratePriorities(size_t nrOfElements, std::promise<std::vector<Priority> > prioritiesRetrievalPromise) // move semantics
{
    prioritiesRetrievalPromise.set_value(generateListElementPriorities(nrOfElements));
}

//void wrapperGeneratePriorities(size_t nrOfElements, std::promise<std::vector<Priority> >* prioritiesRetrievalPromise) // pointer
//{
//    prioritiesRetrievalPromise->set_value(generateListElementPriorities(nrOfElements));
//}

//void wrapperGeneratePriorities(size_t nrOfElements, std::promise<std::vector<Priority> >& prioritiesRetrievalPromise) // reference
//{
//    prioritiesRetrievalPromise.set_value(generateListElementPriorities(nrOfElements));
//}
