#include "bitoperations.h"
#include "error.h"

#define BYTE_SIZE 8

unsigned char rotateLeft(const unsigned char byte, const size_t nrOfRotations)
{
    unsigned char result = byte;

    if (nrOfRotations > 0)
    {
        size_t remainingRotations = nrOfRotations % BYTE_SIZE;
        const unsigned char lsbMask = retrieveSingleBitMask(0);

        while (remainingRotations > 0)
        {
            boolean firstBitSetBeforeShift = isBitSet(result, BYTE_SIZE - 1);
            result = (unsigned char)(result << 1);

            if (firstBitSetBeforeShift)
            {
                result = result | lsbMask;
            }

            --remainingRotations;
        }
    }

    return result;
}

unsigned char rotateRight(const unsigned char byte, const size_t nrOfRotations)
{
    unsigned char result = byte;

    if (nrOfRotations > 0)
    {
        size_t remainingRotations = nrOfRotations % BYTE_SIZE;
        const unsigned char msbMask = retrieveSingleBitMask(BYTE_SIZE - 1);

        while (remainingRotations > 0)
        {
            boolean lastBitSetBeforeShift = isBitSet(result, 0);
            result = result >> 1;

            if (lastBitSetBeforeShift)
            {
                result = result | msbMask;
            }

            --remainingRotations;
        }
    }

    return result;
}

unsigned char swapBits(const unsigned char byte, const unsigned char firstBitIndex, const unsigned char secondBitIndex)
{
    unsigned char result = byte;

    if (firstBitIndex != secondBitIndex)
    {
        if (isBitSet(byte, firstBitIndex) && !isBitSet(byte, secondBitIndex))
        {
            result = resetBit(result, firstBitIndex);
            result = setBit(result, secondBitIndex);
        }
        else if (!isBitSet(byte, firstBitIndex) && isBitSet(byte, secondBitIndex))
        {
            result = setBit(result, firstBitIndex);
            result = resetBit(result, secondBitIndex);
        }
    }

    return result;
}

unsigned char retrieveSingleBitMask(const unsigned char bitIndex)
{
    ASSERT_CONDITION(bitIndex < BYTE_SIZE, "Bit number out of range");
    return (unsigned char)(1 << bitIndex);
}

unsigned char setBit(const unsigned char byte, const unsigned char bitIndex)
{
    return byte | retrieveSingleBitMask(bitIndex);
}

unsigned char resetBit(const unsigned char byte, const unsigned char bitIndex)
{
    return byte & (~retrieveSingleBitMask(bitIndex));
}

unsigned char invertBit(const unsigned char byte, const unsigned char bitIndex)
{
    return isBitSet(byte, bitIndex) ? resetBit(byte, bitIndex) : setBit(byte, bitIndex);
}

size_t getNumberOfSetBits(const unsigned char byte)
{
    size_t result = 0;
    unsigned int temp = byte;
    const unsigned int lsbMask = 1u;

    for (size_t bitNumber = 0; bitNumber < BYTE_SIZE; ++bitNumber)
    {
        if ((temp & lsbMask) == lsbMask)
        {
            ++result;
        }
        temp = temp >> 1;
    }

    return result;
}

size_t getMinimumNrOfBits(const unsigned char byte)
{
    size_t result = 0;
    unsigned char temp = byte;

    do
    {
        ++result;
        temp = temp >> 1;

    }
    while (temp > 0);

    return result;
}

boolean isBitSet(const unsigned char byte, const unsigned char bitIndex)
{
    ASSERT_CONDITION(bitIndex < BYTE_SIZE, "Bit number out of range");
    return (byte & retrieveSingleBitMask(bitIndex));
}
