#ifndef OPTICALFLOWTRACKER_H
#define OPTICALFLOWTRACKER_H

#include "coordsystem.h"
#include "videodatabase.h"
#include "pointtrackerbase.h"

class PointTrackerNeighbourOpticalFlow : public PointTrackerBase
{
    Q_OBJECT

protected:
    bool track(Mat8 &prevFrame, Mat8 &nextFrame, Points &prevPoints, Points &nextPoints);

public:
    float outlierDistanceThreshold;
    int neighbourhoodSize;
    int step;

    PointTrackerNeighbourOpticalFlow(float outlierDistanceThreshold = 20, int neighbourhoodWindoSize = 30, int stepWithinWindow = 5, QObject *parent = 0) :
        PointTrackerBase(parent),
        outlierDistanceThreshold(outlierDistanceThreshold),
        neighbourhoodSize(neighbourhoodWindoSize),
        step(stepWithinWindow)
    { }

    MatF trackIntensity(Mat8 &prevFrame, Mat8 &nextFrame);
};

#endif // OPTICALFLOWTRACKER_H
