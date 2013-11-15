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

    Points getRealShapePoints(const Points &controlPoints, int shapeWidth);

    Points getRealShapePoints(const Points &controlPoints, int shapeWidth, VectorF *modulationValues);

    bool isValidShape(Points &/*realPoints*/) { return true; }

    QList<QGraphicsItem*> drawResult(QGraphicsScene *scene, Points &resultPoints);

    P getBasePoint(Points &realPoints);

    P getApexPoint(Points &realPoints);

    QString getInfo();
};

#endif // LONGITUDINALSTRAIN_H
