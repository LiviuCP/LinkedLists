#include <iostream>
#include <thread>
#include <mutex>

#include "buffermanipulation.h"

static std::mutex printMutex;

void consumeBufferSlot(std::string consumerId, size_t nrOfConsumedValues, BoundedBuffer& buffer)
{
    for (size_t index = 0; index < nrOfConsumedValues; ++index)
    {
        value_t consumedValue = buffer.readElement();

        {
            std::lock_guard<std::mutex> printLock{printMutex};
            std::cout << "Consumer " << consumerId << " read value " << consumedValue << " from buffer" << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds{100});
    }
}

void fillBufferSlot(std::string producerId, value_t initialValue, size_t nrOfProducedValues, BoundedBuffer& buffer)
{
    for (size_t index = 0; index < nrOfProducedValues; ++index)
    {
        value_t producedValue = initialValue + index;
        buffer.writeElement(producedValue);

        {
            std::lock_guard<std::mutex> printLock{printMutex};
            std::cout << "Producer " << producerId << " wrote value " << producedValue << " to buffer" << std::endl;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds{100});
    }
}
