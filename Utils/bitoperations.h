#ifndef BITOPERATIONS_H
#define BITOPERATIONS_H

#include <stdlib.h>

#include "codeutils.h"

typedef unsigned char byte_t;

#ifdef __cplusplus
extern "C"{
#endif

byte_t rotateLeft(const byte_t byte, const size_t nrOfRotations);
byte_t rotateRight(const byte_t byte, const size_t nrOfRotations);
byte_t swapBits(const byte_t byte, const size_t firstBitIndex, const size_t secondBitIndex);
byte_t reverseBits(const byte_t byte);

byte_t retrieveSingleBitMask(const size_t bitIndex);
byte_t setBit(const byte_t byte, const size_t bitIndex);
byte_t resetBit(const byte_t byte, const size_t bitIndex);
byte_t invertBit(const byte_t byte, const size_t bitIndex);

size_t getNumberOfSetBits(const byte_t byte);
size_t getMinimumNrOfBits(const byte_t byte);
boolean isBitSet(const byte_t byte, const size_t bitIndex);

#ifdef __cplusplus
}
#endif

#endif // BITOPERATIONS_H
