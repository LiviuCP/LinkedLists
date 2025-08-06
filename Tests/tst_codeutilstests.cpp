#include <QtTest>

#include "codeutils.h"

class CodeUtilsTests : public QObject
{
    Q_OBJECT

private slots:
    void testConvertIntToString();
    void testCopyNCharsToString();
};

void CodeUtilsTests::testConvertIntToString()
{
    char resultingString[21]; // assume int is 64 bit, worst-case scenario
    size_t bound{sizeof(resultingString)}; // bound matches string size

    bool success = convertIntToString(0, resultingString, bound);

    QVERIFY(success);
    QVERIFY(strcmp(resultingString, "0") == 0);

    success = convertIntToString(-1, resultingString, bound);

    QVERIFY(success);
    QVERIFY(strcmp(resultingString, "-1") == 0);

    success = convertIntToString(1, resultingString, bound);

    QVERIFY(success);
    QVERIFY(strcmp(resultingString, "1") == 0);

    success = convertIntToString(2504326, resultingString, bound);

    QVERIFY(success);
    QVERIFY(strcmp(resultingString, "2504326") == 0);

    success = convertIntToString(-45289458, resultingString, bound);

    QVERIFY(success);
    QVERIFY(strcmp(resultingString, "-45289458") == 0);

    // test with smaller bounds, less than minimum required by function
    bound = 11;
    success = convertIntToString(0, resultingString, bound);

    QVERIFY(!success);

    // test with bound 0 to ensure there is no crash
    bound = 0;
    success = convertIntToString(0, resultingString, bound);

    QVERIFY(!success);

    // test with smaller available string, bound matching string size
    char smallerResultingString[3];
    bound = sizeof(smallerResultingString);
    success = convertIntToString(100, smallerResultingString, bound); // no success but no crash either!

    QVERIFY(!success);

    // don't do this, bound should not be higher than available string size (although in this case the provided string is sufficient for a correct conversion)!
    bound = 25;
    success = convertIntToString(-2, smallerResultingString, bound);

    // no crash / error only because the string size was enough for accomodating all chars including the terminating one ('\0')
    QVERIFY(success);
    QVERIFY(strcmp(smallerResultingString, "-2") == 0);
}

void CodeUtilsTests::testCopyNCharsToString()
{
    // source and destination completely separate
    {
        const char* src = "abcdefghijkl";
        char dest[20];

        setNChars(dest, '\0', sizeof(dest));

        QVERIFY(strlen(dest) == 0);

        size_t copiedCharsCount = copyNCharsToString(dest, src, 5);

        QVERIFY(copiedCharsCount == 5);
        QVERIFY(strcmp(dest, "abcde") == 0);

        copiedCharsCount = copyNCharsToString(dest, src, 11);

        QVERIFY(copiedCharsCount == 11);
        QVERIFY(strcmp(dest, "abcdefghijk") == 0);

        copiedCharsCount = copyNCharsToString(dest, src, 8);

        QVERIFY(copiedCharsCount == 8);
        QVERIFY(strcmp(dest, "abcdefgh") == 0);

        copiedCharsCount = copyNCharsToString(dest, src, 0);

        QVERIFY(copiedCharsCount == 0);
        QVERIFY(strcmp(dest, "abcdefgh") == 0);

        copiedCharsCount = copyNCharsToString(dest, src, 14);

        QVERIFY(copiedCharsCount == 12);
        QVERIFY(strcmp(dest, "abcdefghijkl") == 0);

        copiedCharsCount = copyNCharsToString(dest, src, 1);

        QVERIFY(copiedCharsCount == 1);
        QVERIFY(strcmp(dest, "a") == 0);

        copiedCharsCount = copyNCharsToString(dest, src, 12);

        QVERIFY(copiedCharsCount == 12);
        QVERIFY(strcmp(dest, "abcdefghijkl") == 0);

        copiedCharsCount = copyNCharsToString(dest, src, 11);

        QVERIFY(copiedCharsCount == 11);
        QVERIFY(strcmp(dest, "abcdefghijk") == 0);

        copiedCharsCount = copyNCharsToString(dest, &src[10], 1);

        QVERIFY(copiedCharsCount == 1);
        QVERIFY(strcmp(dest, "k") == 0);

        copiedCharsCount = copyNCharsToString(dest, &src[1], 12);

        QVERIFY(copiedCharsCount == 11);
        QVERIFY(strcmp(dest, "bcdefghijkl") == 0);

        copiedCharsCount = copyNCharsToString(&dest[11], &src[5], 4);

        QVERIFY(copiedCharsCount == 4);
        QVERIFY(strcmp(dest, "bcdefghijklfghi") == 0);
    }

    // source and destination completely separate - empty source case
    {
        const char* src = "";
        char dest[10];

        setNChars(dest, '\0', sizeof(dest));

        QVERIFY(strlen(src) == 0);
        QVERIFY(strlen(dest) == 0);

        size_t copiedCharsCount = copyNCharsToString(dest, src, 1);

        QVERIFY(copiedCharsCount == 0);
        QVERIFY(strlen(dest) == 0);

        copiedCharsCount = copyNCharsToString(dest, src, 0);

        QVERIFY(copiedCharsCount == 0);
        QVERIFY(strlen(dest) == 0);
    }

    // source (initially) included in destination - case 1
    {
        char* testString = createStringCopy("abcdefghijklmnop");

        QVERIFY(testString);
        QVERIFY(strcmp(testString, "abcdefghijklmnop") == 0);

        const char* src = &testString[5];
        char* dest = testString;

        size_t copiedCharsCount = copyNCharsToString(dest, src, 5);

        QVERIFY(copiedCharsCount == 0);
        QVERIFY(strcmp(src, "fghijklmnop") == 0);
        QVERIFY(strcmp(dest, "abcdefghijklmnop") == 0);

        copiedCharsCount = copyNCharsToString(dest, src, 4);

        QVERIFY(copiedCharsCount == 4);
        QVERIFY(strcmp(src, "fghijklmnop") == 0);
        QVERIFY(strcmp(dest, "fghi") == 0);

        free(testString);
        testString = NULL;
    }

    // source (initially) included in destination - case 2
    {
        char* testString = createStringCopy("abcdefghijklmnop");
        const char* src = &testString[5];
        char* dest = testString;

        testString[3] = '\0';
        testString[4] = '\0';

        size_t copiedCharsCount = copyNCharsToString(dest, src, 5);

        QVERIFY(copiedCharsCount == 0);
        QVERIFY(strcmp(src, "fghijklmnop") == 0);
        QVERIFY(strcmp(dest, "abc") == 0);

        copiedCharsCount = copyNCharsToString(dest, src, 4);

        QVERIFY(copiedCharsCount == 4);
        QVERIFY(strcmp(src, "fghijklmnop") == 0);
        QVERIFY(strcmp(dest, "fghi") == 0);

        free(testString);
        testString = NULL;
    }

    // destination (initially) included in source
    {
        char* testString = createStringCopy("ponmlkjihgfedcba");

        QVERIFY(testString);
        QVERIFY(strcmp(testString, "ponmlkjihgfedcba") == 0);

        const char* src = testString;
        char* dest = &testString[7];

        size_t copiedCharsCount = copyNCharsToString(dest, src, 1);

        QVERIFY(copiedCharsCount == 0);
        QVERIFY(strcmp(src, "ponmlkjihgfedcba") == 0);
        QVERIFY(strcmp(dest, "ihgfedcba") == 0);

        testString[6] = '\0';

        QVERIFY(strcmp(src, "ponmlk") == 0);
        QVERIFY(strcmp(dest, "ihgfedcba") == 0);

        copiedCharsCount = copyNCharsToString(dest, src, 3);

        QVERIFY(copiedCharsCount == 3);
        QVERIFY(strcmp(src, "ponmlk") == 0);
        QVERIFY(strcmp(dest, "pon") == 0);

        copiedCharsCount = copyNCharsToString(dest, src, 4);

        QVERIFY(copiedCharsCount == 4);
        QVERIFY(strcmp(src, "ponmlk") == 0);
        QVERIFY(strcmp(dest, "ponm") == 0);

        copiedCharsCount = copyNCharsToString(dest, src, 7);

        QVERIFY(copiedCharsCount == 6);
        QVERIFY(strcmp(src, "ponmlk") == 0);
        QVERIFY(strcmp(dest, "ponmlk") == 0);

        copiedCharsCount = copyNCharsToString(dest, src, 5);

        QVERIFY(copiedCharsCount == 5);
        QVERIFY(strcmp(src, "ponmlk") == 0);
        QVERIFY(strcmp(dest, "ponml") == 0);

        copiedCharsCount = copyNCharsToString(dest, src, 6);

        QVERIFY(copiedCharsCount == 6);
        QVERIFY(strcmp(src, "ponmlk") == 0);
        QVERIFY(strcmp(dest, "ponmlk") == 0);

        copiedCharsCount = copyNCharsToString(dest, src, 0);

        QVERIFY(copiedCharsCount == 0);
        QVERIFY(strcmp(src, "ponmlk") == 0);
        QVERIFY(strcmp(dest, "ponmlk") == 0);

        free(testString);
        testString = NULL;
    }
}

QTEST_APPLESS_MAIN(CodeUtilsTests)

#include "tst_codeutilstests.moc"
