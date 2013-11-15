#include "strainresultprocessing.h"

StrainResProcFloatingAvg::StrainResProcFloatingAvg(int kernelSize, QObject *parent)  : StrainResultProcessingBase(parent), kernelSize(kernelSize)
{
    assert(kernelSize >= 1);
    assert(kernelSize % 2 == 1);
}

ShapeMap StrainResProcFloatingAvg::process(ShapeMap &strainResult, int startIndex, int endIndex, const VideoDataClip *)
{
    int pointCount = strainResult.begin().value().size();
    int windowSize = kernelSize/2;

    ShapeMap processingResult(strainResult);

    // i - index of shape that we are processing
    for (int i = startIndex; i < endIndex; i++)
    {
        Points processedShape;

        // j - index of shape within the current window
        VectorF sumX(pointCount);
        VectorF sumY(pointCount);
        int currentWindowSize = 0;
        for (int j = i - windowSize; j <= i + windowSize; j++)
        {
            if (j < startIndex || j >= endIndex) continue;

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

        processingResult[i] = processedShape;
    }

    return processingResult;
}
