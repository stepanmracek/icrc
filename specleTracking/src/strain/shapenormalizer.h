#ifndef SHAPENORMALIZER_H
#define SHAPENORMALIZER_H

#include <QObject>

#include "linalg/common.h"
#include "linalg/backprojectionbase.h"
#include "statisticalshapemodel.h"

class ShapeNormalizerBase : public QObject
{
    Q_OBJECT

public:
    ShapeNormalizerBase(QObject *parent = 0) : QObject(parent) { }

    virtual Points normalize(Points &points, Mat8 &frame) = 0;
};

class ShapeNormalizerShapeModel : public ShapeNormalizerBase
{
    Q_OBJECT

protected:
    StatisticalShapeModel *model;

public:
    ShapeNormalizerShapeModel(StatisticalShapeModel *model, QObject *parent = 0) :
        ShapeNormalizerBase(parent), model(model)
    {
        model->setParent(this);
    }

    StatisticalShapeModel *getShapeModel() const
    {
        return model;
    }
};

class ShapeNormalizerStatisticalShape : public ShapeNormalizerShapeModel
{
    Q_OBJECT

public:
    ShapeNormalizerStatisticalShape(StatisticalShapeModel *model, QObject *parent = 0) :
        ShapeNormalizerShapeModel(model, parent) { }

    Points normalize(Points &points, Mat8 &)
    {
        return model->normalize(points);
    }
};

class ShapeNormalizerIterativeStatisticalShape : public ShapeNormalizerShapeModel
{
    Q_OBJECT

public:
    ShapeNormalizerIterativeStatisticalShape(StatisticalShapeModel *model, QObject *parent = 0) :
        ShapeNormalizerShapeModel(model, parent) { }

    Points normalize(Points &points, Mat8 &)
    {
        return model->iterativeNormalize(points);
    }
};

/*class ShapeNormalizerIterConfStatShape : public ShapeNormalizerShapeModel
{
    Q_OBJECT

public:
    ShapeNormalizerIterConfStatShape(StatisticalShapeModel *model, QObject *parent = 0) :
        ShapeNormalizerShapeModel(model, parent) { }

    Points normalize(Points &points, Mat8 &frame);
};*/

class ShapeNormalizerPass : public ShapeNormalizerBase
{
    Q_OBJECT

private:
    //int numberOfPoints;

public:
    ShapeNormalizerPass(QObject *parent = 0) : ShapeNormalizerBase(parent) { }

    Points normalize(Points &points, Mat8 &)
    {
        return points;
    }
};

#endif // SHAPENORMALIZER_H
