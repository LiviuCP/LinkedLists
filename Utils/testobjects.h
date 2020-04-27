#ifndef TESTOBJECTS_H
#define TESTOBJECTS_H

#include <stdlib.h>

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

Segment* createSegmentPayload(int startX, int startY, int stopX, int stopY);
LocalConditions* createLocalConditionsPayload(int coordinateX, int coordinateY, int temperature, double humidity);

#ifdef __cplusplus
}
#endif

#endif // TESTOBJECTS_H
