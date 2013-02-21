#ifndef POINTTRACKEROPTICALFLOW_H
#define POINTTRACKEROPTICALFLOW_H

#include "pointtrackerbase.h"

class PointTrackerOpticalFlow : public PointTrackerBase
{
    Q_OBJECT

private:
    float outlierDistanceThreshold;

protected:
    bool track(Mat8 &prevFrame, Mat8 &nextFrame, Points &prevPoints, Points &nextPoints);

public:
    PointTrackerOpticalFlow(int outlierDistanceThreshold, QObject *parent = 0) : PointTrackerBase(parent), outlierDistanceThreshold(outlierDistanceThreshold) {}
};

#endif // POINTTRACKEROPTICALFLOW_H
