#ifndef COORDSYSTEM_H
#define COORDSYSTEM_H

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <opencv2/opencv.hpp>
#include <list>

#include "linalg/common.h"

class CoordSystemBase
{
public:
    virtual Mat8 transform(Mat8 &input) = 0;
    virtual P transform(P input) = 0;
    virtual P transform(float inputX, float inputY) = 0;

    Points transformPoints(Points &inputPoints)
    {
        Points result;
        int n = inputPoints.size();
        for (int i = 0; i < n; i++)
        {
            result.push_back(transform(inputPoints[i]));
        }
        return result;
    }

    Points backTransformPoints(Points &inputPoints)
    {
        Points result;
        int n = inputPoints.size();
        for (int i = 0; i < n; i++)
        {
            result.push_back(backTransform(inputPoints[i]));
        }
        return result;
    }

    VectorOfShapes transformShapes(VectorOfShapes &inputShapes)
    {
        VectorOfShapes result;
        for (VectorOfShapes::iterator it = inputShapes.begin(); it != inputShapes.end(); ++it)
        {
            result.push_back(transformPoints(*it));
        }
        return result;
    }

    virtual P backTransform(P input) = 0;
    virtual P backTransform(float inputX, float inputY) = 0;
    virtual QList<QGraphicsItem*> draw(QGraphicsScene *scene) = 0;
};

class CoordSystemPass : public CoordSystemBase
{
public:
    Mat8 transform(Mat8 &input) { return input; }
    P transform(P input) { return input; }
    P transform(float inputX, float inputY) { return P(inputX, inputY); }
    P backTransform(P input) { return input; }
    P backTransform(float inputX, float inputY) { return P(inputX, inputY); }
    QList<QGraphicsItem*> draw(QGraphicsScene *scene) { return QList<QGraphicsItem*>(); }
};

class CoordSystemRadial : public CoordSystemBase
{
public:
    P center; //, arcStart, arcEnd;
    float startDistance, endDistance;
    int resultMatCols, resultMatRows;

    float angleStart, angleStep, angleEnd, distanceStep, dAngle, dDistance;

public:
    CoordSystemRadial();
    CoordSystemRadial(P center, P arcStart, P arcEnd, float startDistance, int resultMatCols, int resultMatRows);
    CoordSystemRadial(P center, float startDistance, float endDistance, float angleStart, float angleEnd,
                      int resultMatCols, int resultMatRows);

    void init(P center, float startDistance, float endDistance, float angleStart, float angleEnd, int resultMatCols, int resultMatRows);
    void init(P center, P arcStart, P arcEnd, float startDistance, int resultMatCols, int resultMatRows);

    Mat8 transform(Mat8 &src);
    P transform(P input);
    P transform(float inputX, float inputY);

    P backTransform(P input);
    P backTransform(float inputX, float inputY);

    static void AnotateAngleDistance(Mat8 &src, P &center, P &arcStart, P &arcEnd);

    QList<QGraphicsItem*> draw(QGraphicsScene *scene);
};

#endif // COORDSYSTEM_H
