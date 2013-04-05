#include "strain.h"

Strain::Strain(ShapeNormalizerBase *shapeNormalizer, int segmentsCount, int pointsPerSegment, QObject *parent) :
    QObject(parent), segmentsCount(segmentsCount), pointsPerSegment(pointsPerSegment)
{
    this->shapeNormalizer = shapeNormalizer;
    shapeNormalizer->setParent(this);
}
