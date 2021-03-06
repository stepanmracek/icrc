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

    PointTrackerNeighbourOpticalFlow(float outlierDistanceThreshold = 20, int neighbourhoodWindowSize = 31, int stepWithinWindow = 5, QObject *parent = 0) :
        PointTrackerBase(parent),
        outlierDistanceThreshold(outlierDistanceThreshold),
        neighbourhoodSize(neighbourhoodWindowSize),
        step(stepWithinWindow)
    { }

    MatF trackIntensity(Mat8 &prevFrame, Mat8 &nextFrame);

    QString getInfo()
    {
        return QString("Optical flow point tracker\n    Outlier distance: %1\n    Window size: %2\n    Step within window: %3\n")
                .arg(outlierDistanceThreshold).arg(neighbourhoodSize).arg(step);
    }
};

#endif // OPTICALFLOWTRACKER_H
