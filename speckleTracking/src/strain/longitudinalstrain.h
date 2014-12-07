#ifndef LONGITUDINALSTRAIN_H
#define LONGITUDINALSTRAIN_H

#include "strain.h"
#include "linalg/spline.h"
#include "shapenormalizer.h"

class LongitudinalStrain : public Strain
{
    Q_OBJECT

private:
    Spline spline;

public:
    LongitudinalStrain(ShapeNormalizerBase *shapeNormalizer, int segmentsCount, int pointsPerSegment, QObject *parent = 0);

    Points getRealShapePoints(const Points &controlPoints, float shapeWidth);

    Points getRealShapePoints(const Points &controlPoints, float shapeWidth, const VectorF *modulationValues, const VectorF *widthVector);

    bool isValidShape(Points &/*realPoints*/) { return true; }

    QList<QGraphicsItem*> drawResult(QGraphicsScene *scene, const Points &resultPoints);

    P getBasePoint(const Points &realPoints);

    P getApexPoint(const Points &realPoints);

    QString getInfo();
};

#endif // LONGITUDINALSTRAIN_H
