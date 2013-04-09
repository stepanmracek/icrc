#ifndef POINTTRACKERCORRELATION_H
#define POINTTRACKERCORRELATION_H

#include "pointtrackerbase.h"
#include "linalg/metrics.h"

class PointTrackerDistance : public PointTrackerBase
{
    Q_OBJECT

private:
    Metrics *metrics;
    int windowSize;

protected:
    bool track(Mat8 &prevFrame, Mat8 &nextFrame, Points &prevPoints, Points &nextPoints);

public:
    PointTrackerDistance(Metrics *metrics, int windowSize, QObject *parent = 0) :
        PointTrackerBase(parent), metrics(metrics), windowSize(windowSize)
    {
        assert(windowSize >= 1);
        assert(windowSize % 2 == 1);
        metrics->setParent(this);
    }

    QString getInfo()
    {
        return QString("Distance point tracker based on: ") + metrics->metaObject()->className() + QString("\n    Window size: %1\n").arg(windowSize);
    }
};

#endif // POINTTRACKERCORRELATION_H
