#include "strain.h"

Strain::Strain(ShapeNormalizerBase *shapeNormalizer, int segmentsCount, int pointsPerSegment, QObject *parent) :
    SerializableObject(parent), segmentsCount(segmentsCount), pointsPerSegment(pointsPerSegment)
{
    this->shapeNormalizer = shapeNormalizer;
    shapeNormalizer->setParent(this);
}

void Strain::serialize(const QString &path)
{
    shapeNormalizer->serialize(path);

    cv::FileStorage storage(path.toStdString(), cv::FileStorage::APPEND);
    storage << "pointsPerSegment" << pointsPerSegment;
    storage << "segmentsCount" << segmentsCount;
}

void Strain::deserialize(const QString &path)
{
    shapeNormalizer->deserialize(path);

    cv::FileStorage storage(path.toStdString(), cv::FileStorage::READ);
    storage["pointsPerSegment"] >> pointsPerSegment;
    storage["segmentsCount"] >> segmentsCount;
}
