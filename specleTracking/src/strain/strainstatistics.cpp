#include "strainstatistics.h"

#include <cassert>

StrainStatistics::StrainStatistics(Strain &strainModel, VectorOfShapes &shapes)
{
    unsigned int shapesCount = shapes.size();
    assert(shapesCount > 0);

    unsigned int pointsCount = shapes.front().size();
    assert (pointsCount > 0);

    strainForSegments = std::vector<VectorF>(strainModel.segmentsCount);

    // Strain
    for (VectorOfShapes::iterator it = shapes.begin(); it != shapes.end(); ++it)
    {
        Points &shape = *it;
        assert(pointsCount == shape.size());

        P base = strainModel.getBasePoint(shape);
        P apex = strainModel.getApexPoint(shape);
        float s = Common::eucl(base, apex);

        for (int segment = 0; segment < strainModel.segmentsCount; segment++)
        {
            int p = segment*(strainModel.pointsPerSegment*3)+1;
            int n = (segment+1)*(strainModel.pointsPerSegment*3)+1;

            float s = Common::eucl(shape[p], shape[n]);
            strainForSegments[segment].push_back(s);
        }

        strain.push_back(s);
    }

    // Strain rate
    strainRate = Common::deltas(strain);
    for (unsigned int i = 0; i < strainModel.segmentsCount; i++)
    {
        strainRateForSegments.push_back(Common::deltas(strainForSegments[i]));
    }
}

VectorF StrainStatistics::beatToBeatVariance(std::vector<StrainStatistics> &statisticsVector, int samplesCount)
{
    std::vector<VectorF> samplesStats;

    for (std::vector<StrainStatistics>::iterator it = statisticsVector.begin(); it != statisticsVector.end(); ++it)
    {
        StrainStatistics &beatStats = *it;

        samplesStats.push_back(VectorF());

        for (int i = 0; i < samplesCount; i++)
        {
            float indexF = beatStats.strain.size() * i / ((float)(samplesCount));
            int indexI = floor(indexF);
            float delta = indexF - indexI;

            float first = beatStats.strain[indexI];
            float second = beatStats.strain[indexI + 1];
            float interpolated = first + delta * (second - first);

            samplesStats.back().push_back(interpolated);
        }
    }
}

/*{
    unsigned int shapesCount = shapes.size();
    assert(shapesCount > 0);

    unsigned int pointsCount = shapes.front().size();
    assert (pointsCount > 0);

    // initialize per-point statistic structures
    strainForPoints = std::vector<VectorF>(pointsCount);

    // Strain
    for (VectorOfShapes::iterator it = shapes.begin(); it != shapes.end(); ++it)
    {
        Points &shape = *it;
        assert(pointsCount == shape.size());

        P center = getCenter(shape);

        float sumOfStrains = 0;
        //float sumOfStrainRates = 0;
        for (unsigned int i = 0; i < pointsCount; i++)
        {
            P &p = shape[i];

            float strainForPoint = Common::eucl(center, p);
            sumOfStrains += strainForPoint;

            strainForPoints[i].push_back(strainForPoint);
        }

        strain.push_back(sumOfStrains/pointsCount);
    }

    // Strain rate
    strainRate = Common::deltas(strain);
    for (unsigned int i = 0; i < pointsCount; i++)
    {
        strainRateForPoints.push_back(Common::deltas(strainForPoints[i]));
    }
}

P StrainStatistics::getCenter(Points &points)
{
    int n = points.size();
    assert(n > 0);

    float x = 0;
    float y = 0;
    for (int i = 0; i < n; i++)
    {
        x += points[i].x;
        y += points[i].y;
    }

    P p;
    p.x = x/n;
    p.y = y/n;
    return p;
}*/
