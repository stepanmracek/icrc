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

    strainForSegments = std::vector<VectorF>(strainModel->segmentsCount);
    std::vector<float> firstStrainForSegments(strainModel->segmentsCount);

    // Strain
    bool first = true;
    float firstStrain;
    for (VectorOfShapes::iterator it = shapes.begin(); it != shapes.end(); ++it)
    {
        Points &shape = *it;
        assert(pointsCount == shape.size());

        P base = strainModel->getBasePoint(shape);
        P apex = strainModel->getApexPoint(shape);
        float s = Common::eucl(base, apex);
        if (first)
        {
            firstStrain = s;
        }
        s = (s - firstStrain)/firstStrain;
        strain.push_back(s);

        for (int segment = 0; segment < strainModel->segmentsCount; segment++)
        {
            int p = segment * (strainModel->pointsPerSegment*3)+1;
            int n = (segment+1) * (strainModel->pointsPerSegment*3)+1;

            float s = Common::eucl(shape[p], shape[n]);
            if (first)
            {
                firstStrainForSegments[segment] = s;
            }
            s = (s - firstStrainForSegments[segment])/firstStrainForSegments[segment];
            strainForSegments[segment].push_back(s);
        }

        first = false;
    }

    // Strain rate
    strainRate = Common::deltas(strain);
    for (unsigned int i = 0; i < strainModel->segmentsCount; i++)
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
    int nextBeatIndex = clip->size();
    if (i < clip->getMetadata()->beatIndicies.size() - 1)
    {
        nextBeatIndex = clip->getMetadata()->beatIndicies[i+1];
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

