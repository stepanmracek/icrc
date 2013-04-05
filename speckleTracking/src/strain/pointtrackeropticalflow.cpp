#include "pointtrackeropticalflow.h"

bool PointTrackerOpticalFlow::track(Mat8 &prevFrame, Mat8 &nextFrame, Points &prevPoints, Points &nextPoints)
{
    int pointCount = prevPoints.size();
    std::vector<unsigned char> status(pointCount);
    VectorF err(pointCount);

    cv::calcOpticalFlowPyrLK(prevFrame, nextFrame, prevPoints, nextPoints, status, err);

    // ignore outliers
    for (int i = 0; i < pointCount; i++)
    {
        if (status[i] == 0 || Common::eucl(prevPoints.at(i), nextPoints.at(i)) > outlierDistanceThreshold)
            nextPoints[i] = prevPoints[i];
    }

    return true;
}
