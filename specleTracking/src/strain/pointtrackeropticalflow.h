#ifndef POINTTRACKEROPTICALFLOW_H
#define POINTTRACKEROPTICALFLOW_H

#include "pointtrackerbase.h"

class PointTrackerOpticalFlow : public PointTrackerBase
{
private:
    float outlierDistanceThreshold;

protected:
    bool track(Mat8 &prevFrame, Mat8 &nextFrame, Points &prevPoints, Points &nextPoints);

public:
    PointTrackerOpticalFlow(int outlierDistanceThreshold) : outlierDistanceThreshold(outlierDistanceThreshold) {}
};

#endif // POINTTRACKEROPTICALFLOW_H
