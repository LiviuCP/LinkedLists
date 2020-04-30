#include "bitoperations.h"

size_t getNumberOfOnes(const unsigned int number)
{
    unsigned int numberCopy = number;
    const unsigned int one = 1u;
    const size_t totalNrOfBits = 8 * sizeof (unsigned int);
    size_t nrOfOnes = 0;

    for (size_t bitNumber = 0; bitNumber < totalNrOfBits; ++bitNumber)
    {
        if ((numberCopy & one) == one)
        {
            ++nrOfOnes;
        }
        numberCopy = numberCopy >> 1;
    }

    return nrOfOnes;
}

