#include "strain.h"

Strain::Strain(ShapeNormalizerBase *shapeNormalizer, int segmentsCount, int pointsPerSegment, QObject *parent) :
    SerializableObject(parent), segmentsCount(segmentsCount), pointsPerSegment(pointsPerSegment)
{
    this->shapeNormalizer = shapeNormalizer;
    shapeNormalizer->setParent(this);
}

void Strain::serialize(cv::FileStorage &storage) const
{
    shapeNormalizer->serialize(storage);
    storage << "pointsPerSegment" << pointsPerSegment;
    storage << "segmentsCount" << segmentsCount;
}

#include <QDebug>

void Strain::deserialize(cv::FileStorage &storage)
{
    shapeNormalizer->deserialize(storage);
    storage["pointsPerSegment"] >> pointsPerSegment;
    storage["segmentsCount"] >> segmentsCount;
}
