#include <QtTest>

#include "../../Utils/bitoperations.h"

class BitOperationsTests : public QObject
{
    Q_OBJECT

public:
    BitOperationsTests();
    ~BitOperationsTests();

private slots:
    void testRotations();
    void testReverseBits();
    void testSwapBits();
    void testSwapNibbles();
    void testSwapBytes();
    void testCountNrOfSetBits();
    void testCountMinNrOfRequiredBits();
};

BitOperationsTests::BitOperationsTests()
{

}

BitOperationsTests::~BitOperationsTests()
{

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

void BitOperationsTests::testReverseBits()
{
    QVERIFY2(reverseBits(0x00) == 0x00, "The bits are not correctly reversed");
    QVERIFY2(reverseBits(0b01001001) == 0b10010010, "The bits are not correctly reversed");
    QVERIFY2(reverseBits(0b01000010) == 0b01000010, "The bits are not correctly reversed");
    QVERIFY2(reverseBits(0xFF) == 0xFF, "The bits are not correctly reversed");
}

void BitOperationsTests::testSwapBits()
{
    QVERIFY2(swapBits(0b01001001, 3, 5) == 0b01100001, "The bits are not correctly swapped");
    QVERIFY2(swapBits(0b01001001, 5, 3) == 0b01100001, "The bits are not correctly swapped"); // check that bit index order is not relevant
    QVERIFY2(swapBits(0b01001001, 1, 5) == 0b01001001, "The bits are not correctly swapped");
    QVERIFY2(swapBits(0b01001001, 3, 6) == 0b01001001, "The bits are not correctly swapped");
    QVERIFY2(swapBits(0b01001001, 3, 3) == 0b01001001, "The bits are not correctly swapped");
}

void BitOperationsTests::testSwapNibbles()
{
    QVERIFY2(swapNibbles(0x00) == 0x00, "The nibbles are not correctly swapped");
    QVERIFY2(swapNibbles(0b01000010) == 0b00100100, "The nibbles are not correctly swapped");
    QVERIFY2(swapNibbles(0b10101010) == 0b10101010, "The nibbles are not correctly swapped");
    QVERIFY2(swapNibbles(0xFF) == 0xFF, "The nibbles are not correctly swapped");
}

void BitOperationsTests::testSwapBytes()
{
    byte_t first{0b00100001};
    byte_t second{0b00100010};

    swapBytes(&first, &second);

    QVERIFY2(first == 0b00100010 && second == 0b00100001, "The two bytes have not been correctly swapped by bitwise operation");
}

void BitOperationsTests::testCountNrOfSetBits()
{
    QVERIFY(getNumberOfSetBits('m') == 5);
    QVERIFY(getNumberOfSetBits(0xFF) == 8);
    QVERIFY(getNumberOfSetBits(0x00) == 0);
}

void BitOperationsTests::testCountMinNrOfRequiredBits()
{
    QVERIFY(getMinNrOfRequiredBits(0x00) == 1);
    QVERIFY(getMinNrOfRequiredBits(0b00100110) == 6);
    QVERIFY(getMinNrOfRequiredBits(0b00000111) == 3);
    QVERIFY(getMinNrOfRequiredBits(0xFF) == 8);
}

QTEST_APPLESS_MAIN(BitOperationsTests)

#include "tst_bitoperationstests.moc"
