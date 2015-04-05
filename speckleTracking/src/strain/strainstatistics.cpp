#include "strainstatistics.h"

#include <QDebug>
#include <QFile>
#include <QTextStream>
#include <cassert>

#include "linalg/vecf.h"
#include "linalg/metrics.h"

StrainStatistics::SegmentStatsResult StrainStatistics::SegmentStatsResult::mean(const QVector<SegmentStatsResult> &stats)
{
    StrainStatistics::SegmentStatsResult result;
    foreach(const SegmentStatsResult &s, stats)
    {
        result.meanCorrelation += s.meanCorrelation;
        result.meanVariance += s.meanVariance;
        result.segmentCorrelation += s.segmentCorrelation;
        result.segmentVariance += s.segmentVariance;
    }
    result.meanCorrelation /= stats.size();
    result.meanVariance /= stats.size();
    result.segmentCorrelation /= stats.size();
    result.segmentVariance /= stats.size();

    return result;
}

StrainStatistics::StrainStatistics(Strain *strainModel, const VectorOfShapes &shapes)
{
    unsigned int shapesCount = shapes.size();
    assert(shapesCount > 0);

    unsigned int pointsCount = shapes.front().size();
    assert (pointsCount > 0);

    // strain for each point
    strainForPoints = std::vector<VectorF>(pointsCount/3 - 1);

    // strain for entire segment
    strainForSegments = std::vector<VectorF>(strainModel->segmentsCount);

    // Strain
    bool first = true;
    float firstD;
    VectorF firstStrainForPoints;
    for (VectorOfShapes::const_iterator it = shapes.begin(); it != shapes.end(); ++it)
    {
        const Points &shape = *it;
        assert(pointsCount == shape.size());

        float totalD = 0;
        for (unsigned int p = 0; p < pointsCount/3 -1; p++)
        {
            float d = Common::eucl(shape[3*p], shape[3*p+3]);
            d += Common::eucl(shape[3*p+1], shape[3*p+4]);
            d += Common::eucl(shape[3*p+2], shape[3*p+5]);
            d /= 3;

            //qDebug() << 3*p << (3*p+1) << (3*p+2) << "/" << pointsCount;
            //float d = (Common::eucl(shape[p], shape[p+1]) + Common::eucl(shape[p+1], shape[p+2]))/2.0;

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
    for (unsigned int i = 0; i < pointsCount/3 - 1; i++)
    {
        strainRateForPoints.push_back(Common::deltas(strainForPoints[i]));
    }
    for (int i = 0; i < strainModel->segmentsCount; i++)
    {
        strainRateForSegments.push_back(Common::deltas(strainForSegments[i]));
    }
}

void writeVecOfVec(const std::string &name, cv::FileStorage &storage, const std::vector<VectorF> &vecOfvec)
{
    storage << name << "[";
    for (const VectorF &vec : vecOfvec)
    {
        storage << vec;
    }
    storage << "]";
}

void StrainStatistics::saveValues(const QString &fileName) const
{
    QFile file(fileName);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream stream(&file);
    for (float v : strain)
    {
        stream << v << "\n";
    }
    stream.flush();
    file.close();

    for (int segment = 0; segment < strainForSegments.size(); segment++)
    {
        QFile file(fileName + "-" + QString::number(segment));
        file.open(QIODevice::WriteOnly | QIODevice::Text);
        QTextStream stream(&file);
        for (float v : strainForSegments[segment])
        {
            stream << v << "\n";
        }
        stream.flush();
        file.close();
    }
}

void StrainStatistics::serialize(cv::FileStorage &storage) const
{
    storage << "strain" << strain;
    storage << "strainRate" << strainRate;

    writeVecOfVec("strainForPoints", storage, strainForPoints);
    writeVecOfVec("strainRateForPoints", storage, strainRateForPoints);
    writeVecOfVec("strainForSegments", storage, strainForSegments);
    writeVecOfVec("strainRateForSegments", storage, strainRateForSegments);

    /*qDebug() << "strain size" << strain.size();
    qDebug() << "sfp size" << strainForPoints.size() << strainForPoints.front().size();
    qDebug() << "sfs size" << strainForSegments.size() << strainForSegments.front().size();
    exit(0);*/
}

void readVecOfVec(const std::string &name, cv::FileStorage &storage, std::vector<VectorF> &vecOfvec)
{
    cv::FileNode node = storage[name];
    for (cv::FileNodeIterator it = node.begin(); it != node.end(); ++it)
    {
        VectorF values;
        (*it) >> values;
        vecOfvec.push_back(values);
    }
}

void StrainStatistics::deserialize(cv::FileStorage &storage)
{
    strain.clear();
    strainRate.clear();
    strainForPoints.clear();
    strainRateForPoints.clear();
    strainForSegments.clear();
    strainRateForSegments.clear();

    storage["strain"] >> strain;
    storage["strainRate"] >> strainRate;
    readVecOfVec("strainForPoints", storage, strainForPoints);
    readVecOfVec("strainRateForPoints", storage, strainRateForPoints);
    readVecOfVec("strainForSegments", storage, strainForSegments);
    readVecOfVec("strainRateForSegments", storage, strainRateForSegments);
}

float StrainStatistics::beatToBeatVariance(const VectorF &firstBeat, const VectorF &secondBeat, int samplesCount, const Metrics &metrics)
{
    VectorF firstSampled = VecF::resample(firstBeat, samplesCount);
    VectorF secondSampled = VecF::resample(secondBeat, samplesCount);

    MatF firstMat = VecF::fromVector(firstSampled);
    MatF secondMat = VecF::fromVector(secondSampled);
    return metrics.distance(firstMat, secondMat);
}

float StrainStatistics::beatToBeatVariance(const StrainStatistics &firstBeat, const StrainStatistics &secondBeat, int samplesCount, const Metrics &metrics)
{
    return beatToBeatVariance(firstBeat.strain, secondBeat.strain, samplesCount, metrics);
}

float StrainStatistics::beatToBeatVariance(const QVector<VectorF> &beats, int samplesCount, const Metrics &metrics)
{
    int n = beats.count();
    float sum = 0.0f;
    int count = 0;
    for (int i = 0; i < (n-1); i++)
    {
        for (int j = i+1; j < n; j++)
        {
            sum += beatToBeatVariance(beats[i], beats[j], samplesCount, metrics);
            count++;
        }
    }
    return sum / count;
}

float StrainStatistics::beatToBeatVariance(const QVector<StrainStatistics> &beats, int samplesCount, const Metrics &metrics)
{
    int n = beats.count();
    float sum = 0.0f;
    int count = 0;
    for (int i = 0; i < (n-1); i++)
    {
        for (int j = i+1; j < n; j++)
        {
            sum += beatToBeatVariance(beats[i], beats[j], samplesCount, metrics);
            count++;
        }
    }
    return sum / count;
}


StrainStatistics StrainStatistics::getOneBeatStats(const VideoDataClip *clip, Strain *strainModel,
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

StrainStatistics StrainStatistics::meanBeatStats(const QVector<StrainStatistics> &beats, int samplesCount)
{
    StrainStatistics result;
    result.strain.resize(samplesCount);
    result.strainRate.resize(samplesCount);

    int pointCount = beats.first().strainForPoints.size();
    result.strainForPoints = std::vector<VectorF>(pointCount, VectorF(samplesCount));
    result.strainRateForPoints = std::vector<VectorF>(pointCount, VectorF(samplesCount));

    int segmentCount = beats.first().strainForSegments.size();
    result.strainForSegments = std::vector<VectorF>(segmentCount, VectorF(samplesCount));
    result.strainRateForSegments = std::vector<VectorF>(segmentCount, VectorF(samplesCount));

    float coef = 1.0f/beats.size();
    foreach(const StrainStatistics &beat, beats)
    {
        VectorF strain = VecF::resample(beat.strain, samplesCount);
        VectorF strainRate = VecF::resample(beat.strainRate, samplesCount);

        for (int i = 0; i < samplesCount; i++)
        {
            result.strain[i] += strain[i] * coef;
            result.strainRate[i] += strainRate[i] * coef;
        }

        for (int point = 0; point < pointCount; point++)
        {
            VectorF strain = VecF::resample(beat.strainForPoints[point], samplesCount);
            VectorF strainRate = VecF::resample(beat.strainRateForPoints[point], samplesCount);

            for (int i = 0; i < samplesCount; i++)
            {
                result.strainForPoints[point][i] += strain[i] * coef;
                result.strainRateForPoints[point][i] += strainRate[i] * coef;
            }
        }

        for (int segment = 0; segment < segmentCount; segment++)
        {
            VectorF strain = VecF::resample(beat.strainForSegments[segment], samplesCount);
            VectorF strainRate = VecF::resample(beat.strainRateForSegments[segment], samplesCount);

            for (int i = 0; i < samplesCount; i++)
            {
                result.strainForSegments[segment][i] += strain[i] * coef;
                result.strainRateForSegments[segment][i] += strainRate[i] * coef;
            }
        }
    }

    return result;
}

StrainStatistics::SegmentStatsResult StrainStatistics::segmentStatistics(const QVector<StrainStatistics> &beats, int samplesCount)
{
    SegmentStatsResult result;

    StrainStatistics mean = meanBeatStats(beats, samplesCount);
    int segmentCount = mean.strainForSegments.size();

    qDebug() << "    segment against mean";
    for (int segment = 0; segment < segmentCount; segment++)
    {
        result.segmentVariance += beatToBeatVariance(mean.strain, mean.strainForSegments[segment], samplesCount, StdDevOfDifferencesMetric());
        result.segmentCorrelation += beatToBeatVariance(mean.strain, mean.strainForSegments[segment], samplesCount, CorrelationMetric());
    }
    result.segmentVariance /= segmentCount;
    result.segmentCorrelation /= segmentCount;
    qDebug() << "      variance:" << result.segmentVariance;
    qDebug() << "      correlation:" << result.segmentCorrelation;

    qDebug() << "    main strain variation";
    result.meanVariance = beatToBeatVariance(beats, samplesCount, StdDevOfDifferencesMetric());
    result.meanCorrelation = beatToBeatVariance(beats, samplesCount, CorrelationMetric());
    qDebug() << "      variance:" << result.meanVariance;
    qDebug() << "      correlation:" << result.meanCorrelation;

    return result;
}

QVector<StrainStatistics> StrainStatistics::getAllBeatsStats(const VideoDataClip *clip, Strain *strainModel,
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

