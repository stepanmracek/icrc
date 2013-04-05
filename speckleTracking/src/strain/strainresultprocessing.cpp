#include "strainresultprocessing.h"

VectorOfShapes StrainResProcFloatingAvg::process(VectorOfShapes &strainResult, VideoDataBase &)
{
    int n = strainResult.size();
    assert(n > 0);
    int pointCount = strainResult[0].size();

    VectorOfShapes processingResult;

    // i - index of shape that we are processing
    for (int i = 0; i < n; i++)
    {
        Points processedShape;

        // j - index of shape within the current window
        VectorF sumX(pointCount);
        VectorF sumY(pointCount);
        int currentWindowSize = 0;
        for (int j = i - windowSize; j <= i + windowSize; j++)
        {
            if (j < 0 || j >= n) continue;

            currentWindowSize ++;
            // k - index of point within current j-th shape
            for (int k = 0; k < pointCount; k++)
            {
                sumX[k] += strainResult[j][k].x;
                sumY[k] += strainResult[j][k].y;
            }
        }

        for (int k = 0; k < pointCount; k++)
        {
            P p(sumX[k]/currentWindowSize, sumY[k]/currentWindowSize);
            processedShape.push_back(p);
        }

        processingResult.push_back(processedShape);
    }

    return processingResult;
}
