#pragma once

#include "boundedbuffer.h"

void consumeBufferSlot(std::string consumerId, size_t nrOfConsumedValues, BoundedBuffer& buffer);
void fillBufferSlot(std::string producerId, value_t initialValue, size_t nrOfProducedValues, BoundedBuffer& buffer);
