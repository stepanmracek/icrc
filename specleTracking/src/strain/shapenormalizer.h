#ifndef SHAPENORMALIZER_H
#define SHAPENORMALIZER_H

#include "linalg/common.h"
#include "linalg/backprojectionbase.h"
#include "statisticalshapemodel.h"

class ShapeNormalizerBase
{
public:
    virtual Points normalize(Points &points, Mat8 &frame) = 0;
    //virtual int getNumberOfPoints() = 0;
};

class ShapeNormalizerStatisticalShape : public ShapeNormalizerBase
{
private:
    StatisticalShapeModel &model;

public:
    ShapeNormalizerStatisticalShape(StatisticalShapeModel &model) : model(model) {}

    Points normalize(Points &points, Mat8 &)
    {
        return model.normalize(points);
    }

    /*int getNumberOfPoints()
    {
        return model.model.getMean().rows/2;
    }*/
};

class ShapeNormalizerIterativeStatisticalShape : public ShapeNormalizerBase
{
private:
    StatisticalShapeModel &model;

public:
    ShapeNormalizerIterativeStatisticalShape(StatisticalShapeModel &model) : model(model) {}

    Points normalize(Points &points, Mat8 &)
    {
        return model.iterativeNormalize(points);
    }

    /*int getNumberOfPoints()
    {
        return model.model.getMean().rows/2;
    }*/
};

class ShapeNormalizerIterConfStatShape : public ShapeNormalizerBase
{
private:
    StatisticalShapeModel &model;

public:
    ShapeNormalizerIterConfStatShape(StatisticalShapeModel &model) : model(model) {}

    Points normalize(Points &points, Mat8 &frame);

    /*int getNumberOfPoints()
    {
        return model.model.getMean().rows/2;
    }*/
};

class ShapeNormalizerPass : public ShapeNormalizerBase
{
private:
    //int numberOfPoints;

public:
    ShapeNormalizerPass(/*int numberOfPoints*/) /*: numberOfPoints(numberOfPoints)*/ {}

    Points normalize(Points &points, Mat8 &)
    {
        return points;
    }

    /*int getNumberOfPoints()
    {
        return numberOfPoints;
    }*/
};

#endif // SHAPENORMALIZER_H
