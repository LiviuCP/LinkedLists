#ifndef BOUNDEDBUFFER_H
#define BOUNDEDBUFFER_H

#include <mutex>
#include <condition_variable>

#include "linkedlist.h"

// for values stored in bounded buffer only (can be combined with size_t index values to get new values)
using value_t = size_t;

class BoundedBuffer
{
public:
    BoundedBuffer() = delete;
    BoundedBuffer(size_t bufferCapacity);

    void writeElement(value_t value);
    value_t readElement();

    ~BoundedBuffer();

private:
    size_t m_BufferCapacity;
    size_t m_FilledSlots;
    List* m_Container;
    ListIterator m_ReadIterator;
    ListIterator m_WriteIterator;
    std::mutex m_BufferLock;
    std::condition_variable m_BufferNotEmpty;
    std::condition_variable m_BufferNotFull;
};

#endif // BOUNDEDBUFFER_H
