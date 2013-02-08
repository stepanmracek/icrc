#include "pointtrackerbase.h"

bool PointTrackerBase::track(std::vector<Mat8> &prevFrames, const VectorF &weights, Mat8 &nextFrame,
                             VectorOfShapes &prevShapes, Points &nextShape)
{
    int n = prevFrames.size();
    assert(n > 0);
    assert(n <= weights.size());
    assert(n == prevShapes.size());

    VectorOfShapes results;
    for(int i = 0; i < n; i++)
    {
        Mat8 &prevFrame = prevFrames[i];
        Points &prevShape = prevShapes[i];
        Points partialResult;
        bool trackResult = track(prevFrame, nextFrame, prevShape, partialResult);
        if (!trackResult) return false;
        results.push_back(partialResult);
    }

    nextShape = averageResults(results, weights);
    return true;
}

Points PointTrackerBase::averageResults(VectorOfShapes &results, const VectorF &weights)
{
    int n = results.size();
    assert(n > 0);
    assert(n <= weights.size());

    float sum = 0;
    for (int i = 0; i < n; i++)
    {
        sum += weights[i];
    }

    Points result;
    int pointCount = results[0].size();
    for (int j = 0; j < pointCount; j++)
    {
        result.push_back(P(0,0));
    }
    for (int i = 0; i < n; i++)
    {
        for (int j = 0; j < pointCount; j++)
        {
            result[j] += (weights[i]/sum) * results[i][j];
        }
    }
    return result;
}
