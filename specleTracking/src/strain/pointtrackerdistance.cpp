#include "pointtrackerdistance.h"

bool PointTrackerDistance::track(Mat8 &prevFrame, Mat8 &nextFrame, Points &prevPoints, Points &nextPoints)
{
    int n = prevPoints.size();
    nextPoints.clear();
    nextPoints.resize(n);
    for (int i = 0; i < n; i++)
    {
        // choose point
        P &p = prevPoints.at(i);
        int px = round(p.x);
        int py = round(p.y);

        // create initial matrix in prevFrame
        MatF prevMat(2*windowSize+1, 2*windowSize+1);
        Common::createWindow(prevFrame, px, py, windowSize, prevMat);
        int rows = prevMat.rows*prevMat.cols;
        prevMat = prevMat.reshape(1, rows);

        MatF nextMat(2*windowSize+1, 2*windowSize+1);
        MatF nextMatInColumn(rows, 1);

        // choose best neighbour window in nextFrame
        int bestX = -1;
        int bestY = -1;
        float bestDistance = 1e300;
        for (int ny = py-windowSize; ny <= py+windowSize; ny++)
        {
            for (int nx = px-windowSize; nx <= px+windowSize; nx++)
            {
                Common::createWindow(nextFrame, nx, ny, windowSize, nextMat);
                nextMatInColumn = nextMat.reshape(1, rows);

                float d = metrics->distance(prevMat, nextMatInColumn);
                if (d < bestDistance)
                {
                    bestX = nx;
                    bestY = ny;
                    bestDistance = d;
                }
            }
        }

        if (bestX != -1 && bestY != -1)
        {
            //std::cout << i << " " << bestX << " " << bestY << std::endl;
            nextPoints[i] = P(bestX, bestY);
        }
        else
        {
            std::cout << i << " something is wrong" << std::endl;
            nextPoints[i] = prevPoints[i];
        }
    }

    return true;
}
