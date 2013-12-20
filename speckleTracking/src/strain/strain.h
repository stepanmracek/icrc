#ifndef STRAIN_H
#define STRAIN_H

#include <QObject>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QList>

#include "linalg/common.h"
#include "coordsystem.h"
#include "shapenormalizer.h"

/**
 * @brief Base class for strain estimation
 */
class Strain : public SerializableObject
{
    Q_OBJECT

private:
    ShapeNormalizerBase *shapeNormalizer;

public:

    int segmentsCount;
    int pointsPerSegment;  

    Strain(ShapeNormalizerBase *shapeNormalizer, int segmentsCount, int pointsPerSegment, QObject *parent = 0);

    void serialize(const QString &path);

    void deserialize(const QString &path);

    ShapeNormalizerBase *getShapeNormalizer()
    {
        return shapeNormalizer;
    }

    void SetShapeNormalizer(ShapeNormalizerBase *shapeNormalizer)
    {
        delete this->shapeNormalizer;
        this->shapeNormalizer = shapeNormalizer;
        shapeNormalizer->setParent(this);
    }

    virtual bool isValidShape(Points &realPoints) = 0;
    virtual Points getRealShapePoints(const Points &controlPoints, float shapeWidth) = 0;
    virtual QList<QGraphicsItem*> drawResult(QGraphicsScene *scene, Points &realPoints) = 0;
    virtual P getBasePoint(Points &realPoints) = 0;
    virtual P getApexPoint(Points &realPoints) = 0;
    virtual QString getInfo() = 0;
};

#endif // STRAIN_H
