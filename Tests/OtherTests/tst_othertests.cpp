#include <QtTest>

#include "../../Utils/bitoperations.h"

class OtherTests : public QObject
{
    Q_OBJECT

public:
    OtherTests();
    ~OtherTests();

private slots:
    void testSingleBitOperations();
    void testSwapBits();
    void testCountNumberOfSetBits();

};

OtherTests::OtherTests()
{

}

OtherTests::~OtherTests()
{

}

void OtherTests::testSingleBitOperations()
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

void OtherTests::testSwapBits()
{
    QVERIFY2(swapBits(0b01001001, 3, 5) == 0b01100001, "The bits are not correctly swapped");
    QVERIFY2(swapBits(0b01001001, 5, 3) == 0b01100001, "The bits are not correctly swapped"); // check that bit index order is not relevant
    QVERIFY2(swapBits(0b01001001, 1, 5) == 0b01001001, "The bits are not correctly swapped");
    QVERIFY2(swapBits(0b01001001, 3, 6) == 0b01001001, "The bits are not correctly swapped");
    QVERIFY2(swapBits(0b01001001, 3, 3) == 0b01001001, "The bits are not correctly swapped");
}

void OtherTests::testCountNumberOfSetBits()
{
    if (sizeof(unsigned int) == 4)
    {
        qInfo("Unsigned int has 4 bytes");
        QVERIFY(getNumberOfOnes(0b11001000000101001111000011010000) == 12);
        QVERIFY(getNumberOfOnes(0xFFFFFFFF) == 32);
    }

    QVERIFY(getNumberOfOnes(0b0011010000111010) == 7);
    QVERIFY(getNumberOfOnes(0xFFFF) == 16);
    QVERIFY(getNumberOfOnes('m') == 5);
    QVERIFY(getNumberOfOnes(0xFF) == 8);
    QVERIFY(getNumberOfOnes(0u) == 0);


}

QTEST_APPLESS_MAIN(OtherTests)

#include "tst_othertests.moc"
