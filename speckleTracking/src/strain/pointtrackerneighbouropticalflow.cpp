#include "pointtrackerneighbouropticalflow.h"

#include <cassert>

bool PointTrackerNeighbourOpticalFlow::track(Mat8 &prevFrame, Mat8 &nextFrame, Points &prevPoints, Points &nextPoints)
{
    nextPoints.clear();

    int n = prevPoints.size();
    int pointCount = 0;
    int trackedPointsPerInputPoint = 0;
    int delta = neighbourhoodSize/2;

    // Create trackedPoints
    Points trackedPoints;
    Points trackingResult;
    for (int i = 0; i < n; i++)
    {
        P &p = prevPoints[i];
        int x = p.x;
        int y = p.y;
        trackedPointsPerInputPoint = 0;
        for (int yy = y - delta; yy <= y + delta; yy += step)
        {
            for (int xx = x - delta; xx <= x + delta; xx += step)
            {
                P trackedPoint(xx, yy);
                trackedPoints.push_back(trackedPoint);

                trackedPointsPerInputPoint++;
                pointCount = 0;
            }
        }
    }

    std::vector<unsigned char> status(pointCount);
    VectorF err(pointCount);

    // track
    cv::calcOpticalFlowPyrLK(prevFrame, nextFrame, trackedPoints, trackingResult, status, err);

    // interpolate result;
    for (int i = 0; i < n; i++)
    {
        P &currentPrevPoint = prevPoints[i];

        VectorF weights;
        float sumWeights = 0.0f;
        Points candidates;

        int startIndex = i * trackedPointsPerInputPoint;
        int endIndex = startIndex + trackedPointsPerInputPoint;
        for (int j = startIndex; j < endIndex; j++)
        {
            P &prev = trackedPoints[j];
            P &next = trackingResult[j];

            if (status[j] == 0 || Common::eucl(prev, next) > outlierDistanceThreshold)
                continue;

            candidates.push_back(next);

            float intensityOnPrev = prevFrame(prev) / 255.0f;
            float intensityOnNext = nextFrame(next) / 255.0f;
            float distance = Common::eucl(currentPrevPoint, prev);
            float weight = intensityOnNext * intensityOnPrev * exp(-(1.0/(2.0*delta))*distance*distance);
            sumWeights += weight;
            weights.push_back(weight);
        }

        int candidatesCount = candidates.size();
        if (candidatesCount == 0 || sumWeights == 0.0f)
        {
            nextPoints.push_back(currentPrevPoint);
        }
        else
        {
            P newPoint(currentPrevPoint);

            for (int c = 0; c < candidatesCount; c++)
            {
                float w = weights[c] / sumWeights;

                float dx = candidates[c].x - currentPrevPoint.x;
                float dy = candidates[c].y - currentPrevPoint.y;
                newPoint.x += w*dx;
                newPoint.y += w*dy;
            }

            nextPoints.push_back(newPoint);
        }
    }

    return true;
}

MatF PointTrackerNeighbourOpticalFlow::trackIntensity(Mat8 &prevFrame, Mat8 &nextFrame)
{
    int rows = prevFrame.rows;
    int cols = prevFrame.cols;

    int step = 5;

    MatF distances(rows/step, cols/step);
    Points prevPoints;
    for (int y = 0; y < rows; y += step)
    {
        for (int x = 0; x < cols; x += step)
        {
            prevPoints.push_back(P(x,y));
        }
    }
    Points nextPoints = prevPoints;

    int n = prevPoints.size();
    std::vector<unsigned char> status(n);
    VectorF err(n);

    cv::calcOpticalFlowPyrLK(prevFrame, nextFrame, prevPoints, nextPoints, status, err);

    for (int i = 0; i < n; i++)
    {
        P &p = prevPoints[i];
        P &n = nextPoints[i];
        int x = p.x;
        int y = p.y;

        if (status[i] == 0 || Common::eucl(p, n) > outlierDistanceThreshold)
        {
            distances(y / step,x / step) = 0;
            continue;
        }

        float d = Common::eucl(p, n);
        distances(y / step, x / step) = d;
    }

    return distances;
}
