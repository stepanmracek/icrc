#ifndef STATISTICALSHAPECHANGES_H
#define STATISTICALSHAPECHANGES_H

#include <list>

#include "linalg/common.h"
#include "linalg/backprojectionbase.h"

class StatisticalShapeChanges
{
public:
    BackProjectionBase &model;
    int framesCount;

    StatisticalShapeChanges(VectorOfShapes &shapes, int framesCount, BackProjectionBase &model);

    Points predict(std::vector<Points> &shapes);

    static std::vector<Points> createDeltas(VectorOfShapes &shapes);
};

#endif // STATISTICALSHAPECHANGES_H
