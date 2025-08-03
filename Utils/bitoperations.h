#ifndef BITOPERATIONS_H
#define BITOPERATIONS_H

#include <stdlib.h>
#include <stdbool.h>

typedef unsigned char byte_t;

#ifdef __cplusplus
extern "C"{
#endif

byte_t rotateLeft(const byte_t byte, const size_t nrOfRotations);
byte_t rotateRight(const byte_t byte, const size_t nrOfRotations);
byte_t reverseBits(const byte_t byte);
byte_t swapBits(const byte_t byte, const size_t firstBitIndex, const size_t secondBitIndex);
byte_t swapNibbles(const byte_t byte);
void swapBytes(byte_t* firstByte, byte_t* secondByte);

size_t getNumberOfSetBits(const byte_t byte);
size_t getMinNrOfRequiredBits(const byte_t byte);
bool isPalyndrome(const byte_t byte);

// a general purpose replacement for memset which is considered insecure by C11 standard (for string use setNChars(), see codeutils.h)
void setNBytes(byte_t* start, byte_t value, size_t count);

#ifdef __cplusplus
}
#endif

#endif // BITOPERATIONS_H
