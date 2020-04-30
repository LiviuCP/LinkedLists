#include <QtTest>

#include "../../Utils/bitoperations.h"

class BitOperationsTests : public QObject
{
    Q_OBJECT

public:
    BitOperationsTests();
    ~BitOperationsTests();

private slots:
    void testSingleBitOperations();
    void testSwapBits();
    void testReverseBits();
    void testSwapNibbles();
    void testRotations();
    void testCountNumberOfSetBits();
    void testMinimumRequiredBits();
};

BitOperationsTests::BitOperationsTests()
{

}

BitOperationsTests::~BitOperationsTests()
{

}

void BitOperationsTests::testSingleBitOperations()
{
    QVERIFY2(setBit(0b01001001, 5) == 0b01101001, "The bit is not correctly set");
    QVERIFY2(setBit(0b01001001, 3) == 0b01001001, "The bit is not correctly set");

    QVERIFY2(resetBit(0b01001001, 5) == 0b01001001, "The bit is not correctly set");
    QVERIFY2(resetBit(0b01001001, 3) == 0b01000001, "The bit is not correctly set");

    QVERIFY2(invertBit(0b01001001, 5) == 0b01101001, "The bit is not correctly inverted");
    QVERIFY2(invertBit(0b01001001, 3) == 0b01000001, "The bit is not correctly inverted");

    QVERIFY(!isBitSet(0b01001001, 5));
    QVERIFY(isBitSet(0b01001001, 3));
}

void BitOperationsTests::testSwapBits()
{
    QVERIFY2(swapBits(0b01001001, 3, 5) == 0b01100001, "The bits are not correctly swapped");
    QVERIFY2(swapBits(0b01001001, 5, 3) == 0b01100001, "The bits are not correctly swapped"); // check that bit index order is not relevant
    QVERIFY2(swapBits(0b01001001, 1, 5) == 0b01001001, "The bits are not correctly swapped");
    QVERIFY2(swapBits(0b01001001, 3, 6) == 0b01001001, "The bits are not correctly swapped");
    QVERIFY2(swapBits(0b01001001, 3, 3) == 0b01001001, "The bits are not correctly swapped");
}

void BitOperationsTests::testReverseBits()
{
    QVERIFY2(reverseBits(0x00) == 0x00, "The bits are not correctly reversed");
    QVERIFY2(reverseBits(0b01001001) == 0b10010010, "The bits are not correctly reversed");
    QVERIFY2(reverseBits(0b01000010) == 0b01000010, "The bits are not correctly reversed");
    QVERIFY2(reverseBits(0xFF) == 0xFF, "The bits are not correctly reversed");
}

void BitOperationsTests::testSwapNibbles()
{
    QVERIFY2(swapNibbles(0x00) == 0x00, "The nibbles are not correctly swapped");
    QVERIFY2(swapNibbles(0b01000010) == 0b00100100, "The nibbles are not correctly swapped");
    QVERIFY2(swapNibbles(0b10101010) == 0b10101010, "The nibbles are not correctly swapped");
    QVERIFY2(swapNibbles(0xFF) == 0xFF, "The nibbles are not correctly swapped");
}

void BitOperationsTests::testRotations()
{
    QVERIFY2(rotateLeft(0b01001001, 0) == 0b01001001, "The bits are not correctly left rotated");
    QVERIFY2(rotateLeft(0b01001001, 1) == 0b10010010, "The bits are not correctly left rotated");
    QVERIFY2(rotateLeft(0b01001001, 4) == 0b10010100, "The bits are not correctly left rotated");
    QVERIFY2(rotateLeft(0b01001001, 7) == 0b10100100, "The bits are not correctly left rotated");
    QVERIFY2(rotateLeft(0b01001001, 8) == 0b01001001, "The bits are not correctly left rotated");
    QVERIFY2(rotateLeft(0b01001001, 11) == 0b01001010, "The bits are not correctly left rotated");

    QVERIFY2(rotateRight(0b01001001, 0) == 0b01001001, "The bits are not correctly right rotated");
    QVERIFY2(rotateRight(0b01001001, 1) == 0b10100100, "The bits are not correctly right rotated");
    QVERIFY2(rotateRight(0b01001001, 4) == 0b10010100, "The bits are not correctly right rotated");
    QVERIFY2(rotateRight(0b01001001, 7) == 0b10010010, "The bits are not correctly right rotated");
    QVERIFY2(rotateRight(0b01001001, 8) == 0b01001001, "The bits are not correctly right rotated");
    QVERIFY2(rotateRight(0b01001001, 11) == 0b00101001, "The bits are not correctly right rotated");
}

void BitOperationsTests::testCountNumberOfSetBits()
{
    QVERIFY(getNumberOfSetBits('m') == 5);
    QVERIFY(getNumberOfSetBits(0xFF) == 8);
    QVERIFY(getNumberOfSetBits(0x00) == 0);
}

void BitOperationsTests::testMinimumRequiredBits()
{
    QVERIFY(getMinimumNrOfBits(0x00) == 1);
    QVERIFY(getMinimumNrOfBits(0b00100110) == 6);
    QVERIFY(getMinimumNrOfBits(0b00000111) == 3);
    QVERIFY(getMinimumNrOfBits(0xFF) == 8);
}

QTEST_APPLESS_MAIN(BitOperationsTests)

#include "tst_bitoperationstests.moc"
