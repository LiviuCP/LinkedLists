#include "bitoperations.h"
#include "error.h"

#define BYTE_SIZE 8
#define NIBBLE_SIZE BYTE_SIZE/2

byte_t rotateLeft(const byte_t byte, const size_t nrOfRotations)
{
    byte_t result = byte;

    if (nrOfRotations > 0)
    {
        size_t remainingRotations = nrOfRotations % BYTE_SIZE;
        const byte_t lsbMask = retrieveSingleBitMask(0);

        while (remainingRotations > 0)
        {
            boolean firstBitSetBeforeShift = isBitSet(result, BYTE_SIZE - 1);
            result = (byte_t)(result << 1);

            if (firstBitSetBeforeShift)
            {
                result = result | lsbMask;
            }

            --remainingRotations;
        }
    }

    return result;
}

byte_t rotateRight(const byte_t byte, const size_t nrOfRotations)
{
    byte_t result = byte;

    if (nrOfRotations > 0)
    {
        size_t remainingRotations = nrOfRotations % BYTE_SIZE;
        const byte_t msbMask = retrieveSingleBitMask(BYTE_SIZE - 1);

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

byte_t swapBits(const byte_t byte, const size_t firstBitIndex, const size_t secondBitIndex)
{
    byte_t result = byte;

    if (firstBitIndex != secondBitIndex)
    {
        const byte_t firstBitMask = (byte_t)(1 << firstBitIndex);
        const byte_t secondBitMask = (byte_t)(1 << secondBitIndex);

        if (((byte & firstBitMask) >> firstBitIndex) ^ ((byte & secondBitMask) >> secondBitIndex))
        {
            result = (~result & firstBitMask) | (result & ~firstBitMask);   // invert first bit
            result = (~result & secondBitMask) | (result & ~secondBitMask); // invert second bit
        }
    }

    return result;
}

byte_t reverseBits(const byte_t byte)
{
    byte_t result = byte;
    byte_t leftMask = 128u;
    byte_t rightMask = 1u;

    for (size_t bitIndex = 0; bitIndex < BYTE_SIZE; ++bitIndex)
    {
        result =(byte & rightMask) == 0u ? result & (~leftMask) : result | leftMask;
        leftMask = leftMask >> 1;
        rightMask = (byte_t)(rightMask << 1);
    }

    return result;
}

byte_t swapNibbles(const byte_t byte)
{
    return (0u | ((byte_t)(byte << NIBBLE_SIZE))) | (byte >> NIBBLE_SIZE);
}

void swapBytes(byte_t* firstByte, byte_t* secondByte)
{
    if (firstByte != NULL && secondByte != NULL && firstByte != secondByte)
    {
        byte_t bitMask = 1;

        for (size_t bitIndex = 0; bitIndex < BYTE_SIZE; ++bitIndex)
        {
            if ((bitMask & *firstByte) ^ (bitMask & *secondByte))
            {
                *firstByte = (~(*firstByte) & bitMask) | ((*firstByte) & (~bitMask));
                *secondByte = ((~*secondByte) & bitMask) | ((*secondByte) & (~bitMask));
                bitMask = (byte_t)(bitMask << 1);
            }
        }
    }
}

byte_t retrieveSingleBitMask(const size_t bitIndex)
{
    ASSERT_CONDITION(bitIndex < BYTE_SIZE, "Bit number out of range");
    return (byte_t)(1 << bitIndex);
}

byte_t setBit(const byte_t byte, const size_t bitIndex)
{
    return byte | retrieveSingleBitMask(bitIndex);
}

byte_t resetBit(const byte_t byte, const size_t bitIndex)
{
    return byte & (~retrieveSingleBitMask(bitIndex));
}

byte_t invertBit(const byte_t byte, const size_t bitIndex)
{
    return isBitSet(byte, bitIndex) ? resetBit(byte, bitIndex) : setBit(byte, bitIndex);
}

size_t getNumberOfSetBits(const byte_t byte)
{
    size_t result = 0;
    byte_t temp = byte;
    const byte_t lsbMask = 1u;

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

size_t getMinimumNrOfBits(const byte_t byte)
{
    size_t result = 0;
    byte_t temp = byte;

    do
    {
        ++result;
        temp = temp >> 1;

    }
    while (temp > 0);

    return result;
}

boolean isBitSet(const byte_t byte, const size_t bitIndex)
{
    ASSERT_CONDITION(bitIndex < BYTE_SIZE, "Bit number out of range");
    return (byte & retrieveSingleBitMask(bitIndex));
}
