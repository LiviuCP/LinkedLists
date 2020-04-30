#include <QtTest>

#include "../../Utils/bitoperations.h"

class OtherTests : public QObject
{
    Q_OBJECT

public:
    OtherTests();
    ~OtherTests();

private slots:
    void testCountNumberOfSetBits();

};

OtherTests::OtherTests()
{

}

OtherTests::~OtherTests()
{

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
