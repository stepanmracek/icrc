#ifndef POINTTRACKERCORRELATION_H
#define POINTTRACKERCORRELATION_H

#include "pointtrackerbase.h"
#include "linalg/metrics.h"

class PointTrackerDistance : public PointTrackerBase
{
private:
    Metrics &metrics;
    int windowSize;

protected:
    bool track(Mat8 &prevFrame, Mat8 &nextFrame, Points &prevPoints, Points &nextPoints);

public:
    PointTrackerDistance(Metrics &metrics, int windowSize) : metrics(metrics), windowSize(windowSize) {}
};

#endif // POINTTRACKERCORRELATION_H
