#ifndef POINTTRACKERCORRELATION_H
#define POINTTRACKERCORRELATION_H

#include "pointtrackerbase.h"
#include "linalg/metrics.h"

class PointTrackerDistance : public PointTrackerBase
{
    Q_OBJECT

private:
    Metrics &metrics;
    int windowSize;

protected:
    bool track(Mat8 &prevFrame, Mat8 &nextFrame, Points &prevPoints, Points &nextPoints);

public:
    PointTrackerDistance(Metrics &metrics, int windowSize, QObject *parent = 0) : PointTrackerBase(parent), metrics(metrics), windowSize(windowSize) { }
};

#endif // POINTTRACKERCORRELATION_H
