#include "imagefilter.h"

#include "linalg/metrics.h"

VectorOfImages ImageFilterBase::calculateMean(VectorOfImages &inputImages, int windowSize)
{
    int n = inputImages.size();
    assert(windowSize < n);
    assert(windowSize >= 2);
    assert(n >= 3);

    VectorOfImages result;
    int rows = inputImages[0].rows;
    int cols = inputImages[0].cols;

    for (int i = 0; i < (n - windowSize); i++)
    {
        MatF partialResultF = MatF::zeros(rows, cols);
        for (int j = 0; j < windowSize; j++)
        {
            int frameIndex = i+j;
            Mat8 &frame = inputImages[frameIndex];
            MatF frameF;
            frame.convertTo(frameF, CV_32FC1);
            partialResultF += frameF;
        }
        partialResultF /= windowSize;

        Mat8 partialResult;
        partialResultF.convertTo(partialResult, CV_8UC1);
        result.push_back(partialResult);
    }

    return result;
}

/*void ImageFilterNlMeansDenoise::process(Mat8 &target)
{
    Mat8 source = target.clone();

    //int searchCount = (searchSize*2+1)*(searchSize*2+1);
    int templateCount = (templateSize*2+1)*(templateSize*2+1);

    MatF Ni(templateSize*2+1,templateSize*2+1);
    MatF Nj(templateSize*2+1,templateSize*2+1);
    MatF NiCol(templateCount, 1);
    MatF NjCol(templateCount, 1);

    EuclideanMetric eucl;
    float h2 = 1/(h*h);

    // for each point in the frame
    for (int y = 0; y < source.rows; y++)
    {
        for (int x = 0; x < source.cols; x++)
        {
            Common::createWindow(source, x, y, templateSize, Ni);
            NiCol = Ni.reshape(1, templateCount);

            // search in the search window
            MatF distancesMatrix(searchSize*2+1, searchSize*2+1);
            int distanceMatRow = 0;
            int distanceMatCol = 0;
            float Z = 0;
            for (int yy = y - searchSize; yy <= y + searchSize; yy++)
            {
                distanceMatCol = 0;
                for (int xx = x - searchSize; xx <= x + searchSize; xx++)
                {
                    // window around second point
                    Common::createWindow(source, xx, yy, templateSize, Nj);
                    NjCol = Nj.reshape(1, templateCount);

                    // calculate distance
                    float d = eucl.distance(NiCol, NjCol);
                    float z = exp(-d * h2);
                    Z += z;
                    distancesMatrix(distanceMatRow, distanceMatCol) = z;

                    distanceMatCol++;
                }
                distanceMatRow++;
            }

            float invZ = 1/Z;
            distancesMatrix = distancesMatrix * invZ;

            float targetIntensity = 0;
            distanceMatRow = 0;
            distanceMatCol = 0;
            for (int yy = y - searchSize; yy <= y + searchSize; yy++)
            {
                distanceMatCol = 0;
                for (int xx = x - searchSize; xx <= x + searchSize; xx++)
                {
                    float intensity = 0;
                    if (yy >= 0 && xx >= 0 && yy < source.rows && xx < source.cols)
                        intensity = source(yy, xx);

                    targetIntensity += distancesMatrix(distanceMatRow, distanceMatCol) * intensity;

                    distanceMatCol++;
                }
                distanceMatRow++;
            }
            target(y, x) = targetIntensity;
        }
    }
}*/


