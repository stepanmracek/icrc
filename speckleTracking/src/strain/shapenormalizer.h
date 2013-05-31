#ifndef SHAPENORMALIZER_H
#define SHAPENORMALIZER_H

#include <QObject>

#include "linalg/common.h"
#include "linalg/backprojectionbase.h"
#include "statisticalshapemodel.h"
#include "linalg/serializableobject.h"

class ShapeNormalizerBase : public SerializableObject
{
    Q_OBJECT

public:
    ShapeNormalizerBase(QObject *parent = 0) : SerializableObject(parent) { }

    virtual Points normalize(const Points &points, const Mat8 &frame) = 0;

    virtual QString getInfo() = 0;
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

    void serialize(const QString &path)
    {
        model->serialize(path);
    }

    void deserialize(const QString &path)
    {
        model->deserialize(path);
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

    Points normalize(const Points &points, const Mat8 &)
    {
        return model->normalize(points);
    }

    QString getInfo()
    {
        return QString("Statistical shape normalizer using ") + model->getBackProjection()->metaObject()->className();
    }
};

class ShapeNormalizerIterativeStatisticalShape : public ShapeNormalizerShapeModel
{
    Q_OBJECT

public:
    ShapeNormalizerIterativeStatisticalShape(StatisticalShapeModel *model, QObject *parent = 0) :
        ShapeNormalizerShapeModel(model, parent) { }

    Points normalize(const Points &points, const Mat8 &)
    {
        return model->iterativeNormalize(points);
    }

    QString getInfo()
    {
        return QString("Iterative statistical shape normalizer using ") + model->getBackProjection()->metaObject()->className();
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

    void serialize(const QString &) {}
    void deserialize(const QString &) {}

    Points normalize(const Points &points, const Mat8 &)
    {
        return points;
    }

    QString getInfo()
    {
        return QString("No shape normalizer");
    }
};

#endif // SHAPENORMALIZER_H
