#include <iostream>
#include <thread>

#include "asyncutils.h"

int main()
{
    using namespace std;

    size_t nrOfElements{requestInputFromUser()};
    system("clear");

    std::vector<size_t> receivedPriorities{};
    if (nrOfElements > 0)
    {
        cout << "Launching async task...";
        std::future<std::vector<size_t>> result{std::async(std::launch::async, generateListElementPriorities, nrOfElements)};
        cout << "DONE" << endl;
        std::this_thread::sleep_for(std::chrono::milliseconds{500});
        cout << "Waiting for result..." << endl << endl;
        std::this_thread::sleep_for(std::chrono::milliseconds{500});

        getDataFromTask(result, receivedPriorities);

        if (receivedPriorities.size() > 0)
        {
            putPrioritiesIntoList(receivedPriorities);
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
