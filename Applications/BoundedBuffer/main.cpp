#include <iostream>
#include <thread>

#include "boundedbuffer.h"
#include "buffermanipulation.h"

int main()
{
    try
    {
        BoundedBuffer buffer{150};

        std::thread producerThread1{fillBufferSlot, "p_1_ 100", 100, 155, std::ref(buffer)};
        std::thread producerThread2{fillBufferSlot, "p_2_1000", 1000, 110, std::ref(buffer)};
        std::thread consumerThread1{consumeBufferSlot, "c_1", 70, std::ref(buffer)};
        std::thread consumerThread2{consumeBufferSlot, "c_2", 80, std::ref(buffer)};
        std::thread consumerThread3{consumeBufferSlot, "c_2", 115, std::ref(buffer)};

        producerThread1.join();
        producerThread2.join();
        consumerThread1.join();
        consumerThread2.join();
        consumerThread3.join();

        return 0;
    }
    catch(std::runtime_error& err)
    {
        std::cout << "Error! " << err.what() << std::endl;
        exit(-1);
    }
}
