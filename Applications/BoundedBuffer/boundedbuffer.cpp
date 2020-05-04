#include "boundedbuffer.h"
#include "../../Utils/error.h"

BoundedBuffer::BoundedBuffer(size_t bufferCapacity)
    : m_BufferCapacity{bufferCapacity}
    , m_FilledSlots{0}
{
    ASSERT_CONDITION(m_BufferCapacity > 0, "Requested capacity of the bounded buffer is 0");

    m_Container = createList();

    if (m_Container != nullptr)
    {
        for (size_t index = 0; index < m_BufferCapacity; ++index)
        {
            ListElement* appendedElement{createAndAppendToList(m_Container, 0)}; // all elements with priority 0 by default, this will get modified with reads and writes

            if (appendedElement == nullptr)
            {
                throw std::runtime_error{"Unable to reserve the required buffer capacity"};
            }
        }

        m_ReadIterator = lbegin(m_Container);
        m_WriteIterator = lbegin(m_Container);
    }
    else
    {
        throw std::runtime_error{"Unable to create the buffer"};
    }
}

void BoundedBuffer::writeElement(value_t value)
{
    std::unique_lock<std::mutex> writeLock{m_BufferLock};

    m_BufferNotFull.wait(writeLock, [this](){return m_FilledSlots != m_BufferCapacity;});
    m_WriteIterator.current->priority = value;
    ++m_FilledSlots;
    lnext(&m_WriteIterator);

    if (areIteratorsEqual(m_WriteIterator, lend(m_Container)))
    {
        m_WriteIterator = lbegin(m_Container);
    }

    writeLock.unlock();
    m_BufferNotEmpty.notify_one();
}

value_t BoundedBuffer::readElement()
{
    std::unique_lock<std::mutex> readLock{m_BufferLock};

    m_BufferNotEmpty.wait(readLock, [this]{return  m_FilledSlots != 0;});
    value_t value = m_ReadIterator.current->priority;
    --m_FilledSlots;
    lnext(&m_ReadIterator);

    if (areIteratorsEqual(m_ReadIterator, lend(m_Container)))
    {
        m_ReadIterator = lbegin(m_Container);
    }

    readLock.unlock();
    m_BufferNotFull.notify_one();

    return value;
}

BoundedBuffer::~BoundedBuffer()
{
    deleteList(m_Container, deleteObject);
    m_Container = nullptr;
}
