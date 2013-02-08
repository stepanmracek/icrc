#ifndef STRAIN_H
#define STRAIN_H

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QList>

#include "linalg/common.h"
#include "coordsystem.h"
#include "shapenormalizer.h"

class Strain
{
public:
    int segmentsCount;
    int pointsPerSegment;

    ShapeNormalizerBase &shapeNormalizer;

    Strain(ShapeNormalizerBase &shapeNormalizer,
           int segmentsCount, int pointsPerSegment) :
        shapeNormalizer(shapeNormalizer), segmentsCount(segmentsCount),
        pointsPerSegment(pointsPerSegment) {}

    virtual bool isValidShape(Points &realPoints) = 0;
    virtual Points getRealShapePoints(Points &controlPoints, int shapeWidth) = 0;
    virtual QList<QGraphicsItem*> drawResult(QGraphicsScene *scene, Points &realPoints) = 0;
    virtual P getBasePoint(Points &realPoints) = 0;
    virtual P getApexPoint(Points &realPoints) = 0;
};

#endif // STRAIN_H
