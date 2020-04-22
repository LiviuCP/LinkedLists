#ifndef CODEUTILS_H
#define CODEUTILS_H

#include <stdlib.h>

typedef unsigned int boolean;

#define TRUE 1u
#define FALSE 0u

typedef struct {
    int x;
    int y;
} Point;

typedef struct {
    Point* start;
    Point* stop;
} Segment;

typedef struct {
    Point* position;
    int temperature;
    double humidity;
} LocalConditions;

#ifdef __cplusplus
extern "C"{
#endif

char* getLine();
int readUnsignedLong(size_t* number);
int isUnsignedLong(const char* input);

Segment* createSegmentPayload(int startX, int startY, int stopX, int stopY);
LocalConditions* createLocalConditionsPayload(int coordinateX, int coordinateY, int temperature, double humidity);

#ifdef __cplusplus
}
#endif

#endif // CODEUTILS_H
