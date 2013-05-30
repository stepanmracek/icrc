#include "strainstatistics.h"

#include <QDebug>
#include <cassert>

#include "linalg/vecf.h"

StrainStatistics::StrainStatistics(Strain *strainModel, VectorOfShapes &shapes)
{
    unsigned int shapesCount = shapes.size();
    assert(shapesCount > 0);

    unsigned int pointsCount = shapes.front().size();
    assert (pointsCount > 0);
    qDebug() << "StrainStatistics";
    qDebug() << "pointsCount" << pointsCount << pointsCount/3 -1;
    qDebug() << "points per segment" << strainModel->pointsPerSegment;
    qDebug() << "segments" << strainModel->segmentsCount;

    strainForPoints = std::vector<VectorF>(pointsCount/3 - 1);
    strainForSegments = std::vector<VectorF>(strainModel->segmentsCount);

    // Strain
    bool first = true;
    float firstD;
    VectorF firstStrainForPoints;
    for (VectorOfShapes::iterator it = shapes.begin(); it != shapes.end(); ++it)
    {
        Points &shape = *it;
        assert(pointsCount == shape.size());

        float totalD = 0;
        for (int p = 0; p < pointsCount/3 - 1; p++)
        {
            float d = Common::eucl(shape[p], shape[p+3]);
            d += Common::eucl(shape[p+1], shape[p+4]);
            d += Common::eucl(shape[p+2], shape[p+5]);
            d /= 3;

            if (first)
            {
                firstStrainForPoints.push_back(d);
            }

            strainForPoints[p].push_back((d - firstStrainForPoints[p]) / firstStrainForPoints[p]);
            totalD += d;
        }

        for (int i = 0; i < strainModel->segmentsCount; i++)
        {
            int startPointIndex = i*strainModel->pointsPerSegment;
            float segmentStrain = 0;
            for (int p = startPointIndex; p < startPointIndex + strainModel->pointsPerSegment; p++)
            {
                segmentStrain += strainForPoints[p].back();
            }
            segmentStrain /= strainModel->pointsPerSegment;
            strainForSegments[i].push_back(segmentStrain);
        }

        if (first)
        {
            first = false;
            firstD = totalD;
        }

        float s = (totalD - firstD)/ firstD;
        strain.push_back(s);
    }

    // Strain rate
    strainRate = Common::deltas(strain);
    for (int i = 0; i < pointsCount/3 - 1; i++)
    {
        strainRateForPoints.push_back(Common::deltas(strainForPoints[i]));
    }
    for (int i = 0; i < strainModel->segmentsCount; i++)
    {
        strainRateForSegments.push_back(Common::deltas(strainForSegments[i]));
    }
}

float StrainStatistics::beatToBeatVariance(StrainStatistics &firstBeat, StrainStatistics &secondBeat, int samplesCount)
{
    VectorF firstSampled = VecF::resample(firstBeat.strain, samplesCount);
    VectorF secondSampled = VecF::resample(secondBeat.strain, samplesCount);

    MatF firstMat = VecF::fromVector(firstSampled);
    MatF secondMat = VecF::fromVector(secondSampled);
    MatF delta = firstMat - secondMat;
    return VecF::stdDeviation(delta);
}

StrainStatistics StrainStatistics::getOneBeatStats(VideoDataClip *clip, Strain *strainModel,
                                                   int beatIndex, ShapeMap &shapesMap, bool *success)
{
    if (beatIndex >= clip->size())
    {
        (*success) = false;
        return StrainStatistics();
    }

    // check if beat exists
    int i = clip->getMetadata()->beatIndicies.indexOf(beatIndex);
    if (i == -1)
    {
        (*success) = false;
        return StrainStatistics();
    }

    // get next beat index
    int nextBeatIndex = clip->size()-1;
    if (i < clip->getMetadata()->beatIndicies.size() - 1)
    {
        nextBeatIndex = clip->getMetadata()->beatIndicies[i+1];
    }

    int delta = nextBeatIndex-beatIndex;
    if (delta < 1)
    {
        (*success) = false;
        return StrainStatistics();
    }

    // check if entire beat has corresponding shapes
    VectorOfShapes shapes;
    for (int index = beatIndex; index < nextBeatIndex; index++)
    {
        if (!shapesMap.contains(index))
        {
            (*success) = false;
            return StrainStatistics();
        }

        shapes.push_back(shapesMap[index]);
    }

    (*success) = true;
    return StrainStatistics(strainModel, shapes);
}

QVector<StrainStatistics> StrainStatistics::getAllBeatsStats(VideoDataClip *clip, Strain *strainModel,
                                                             ShapeMap &shapesMap)
{
    QVector<StrainStatistics> result;

    //qDebug() << "getAllBeatsStats()";
    foreach(int beatIndex, clip->getMetadata()->beatIndicies)
    {
        //qDebug() << "  beat index:" << beatIndex;
        bool success;
        StrainStatistics stats = StrainStatistics::getOneBeatStats(clip, strainModel, beatIndex, shapesMap, &success);
        if (success)
        {
            //qDebug() << "  success, beat length:" << stats.strain.size();
            result << stats;
        }
    }

    //qDebug() << "calculated for" << result.count() << "beats";
    return result;
}

