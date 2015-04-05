#ifndef STRAINSTATISTICS_H
#define STRAINSTATISTICS_H

#include <list>

#include "linalg/common.h"
#include "strain.h"
#include "videodataclip.h"
#include "linalg/serializableobject.h"
#include "linalg/metrics.h"

class StrainStatistics
{
public:
    struct SegmentStatsResult
    {
        SegmentStatsResult() :
            segmentVariance(0.0f), segmentCorrelation(0.0f),
            meanVariance(0.0f), meanCorrelation(0.0f) { }

        float segmentVariance;
        float segmentCorrelation;
        float meanVariance;
        float meanCorrelation;

        static SegmentStatsResult mean(const QVector<SegmentStatsResult> &stats);
    };

    VectorF strain;
    VectorF strainRate;

    std::vector<VectorF> strainForPoints;
    std::vector<VectorF> strainForSegments;
    std::vector<VectorF> strainRateForPoints;
    std::vector<VectorF> strainRateForSegments;

    StrainStatistics() { }
    StrainStatistics(Strain *strainModel, const VectorOfShapes &shapes);

    void saveValues(const QString &fileName) const;
    void serialize(cv::FileStorage &storage) const;
    void deserialize(cv::FileStorage &storage);

    static float beatToBeatVariance(const VectorF &firstBeat, const VectorF &secondBeat, int samplesCount,
                                    const Metrics &metrics);

    static float beatToBeatVariance(const StrainStatistics &firstBeat, const StrainStatistics &secondBeat, int samplesCount,
                                    const Metrics &metrics);

    static float beatToBeatVariance(const QVector<VectorF> &beats, int samplesCount,
                                    const Metrics &metrics);

    static float beatToBeatVariance(const QVector<StrainStatistics> &beats, int samplesCount,
                                    const Metrics &metrics);


    static SegmentStatsResult segmentStatistics(const QVector<StrainStatistics> &beats, int samplesCount);

    static StrainStatistics meanBeatStats(const QVector<StrainStatistics> &beats, int samplesCount);

    static StrainStatistics getOneBeatStats(const VideoDataClip *clip, Strain *strainModel,
                                            int beatIndex, ShapeMap &shapesMap, bool *success);

    static QVector<StrainStatistics> getAllBeatsStats(const VideoDataClip *clip, Strain *strainModel,
                                                      ShapeMap &shapesMap);
};

#endif // STRAINSTATISTICS_H
