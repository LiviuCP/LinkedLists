#ifndef BITOPERATIONS_H
#define BITOPERATIONS_H

#include <stdlib.h>

#include "codeutils.h"

#ifdef __cplusplus
extern "C"{
#endif

unsigned char rotateLeft(const unsigned char byte, const size_t nrOfRotations);
unsigned char rotateRight(const unsigned char byte, const size_t nrOfRotations);
unsigned char swapBits(const unsigned char byte, const unsigned char firstBitIndex, const unsigned char secondBitIndex);

unsigned char retrieveSingleBitMask(const unsigned char bitIndex);
unsigned char setBit(const unsigned char byte, const unsigned char bitIndex);
unsigned char resetBit(const unsigned char byte, const unsigned char bitIndex);
unsigned char invertBit(const unsigned char byte, const unsigned char bitIndex);

size_t getNumberOfSetBits(const unsigned char byte);
size_t getMinimumNrOfBits(const unsigned char byte);
boolean isBitSet(const unsigned char byte, const unsigned char bitIndex);

#ifdef __cplusplus
}
#endif

#endif // BITOPERATIONS_H
