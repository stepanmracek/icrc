#ifndef STRAINSTATISTICS_H
#define STRAINSTATISTICS_H

#include <list>

#include "linalg/common.h"
#include "strain.h"
#include "videodataclip.h"
#include "linalg/serializableobject.h"

class StrainStatistics
{
public:
    VectorF strain;
    VectorF strainRate;

    std::vector<VectorF> strainForPoints;
    std::vector<VectorF> strainForSegments;
    std::vector<VectorF> strainRateForPoints;
    std::vector<VectorF> strainRateForSegments;

    StrainStatistics() { }
    StrainStatistics(Strain *strainModel, const VectorOfShapes &shapes);

    void serialize(cv::FileStorage &storage) const;
    void deserialize(cv::FileStorage &storage);

    static float beatToBeatVariance(const VectorF &firstBeat, const VectorF &secondBeat, int samplesCount);
    static float beatToBeatVariance(StrainStatistics &firstBeat, StrainStatistics &secondBeat, int samplesCount);
    static float beatToBeatVariance(QVector<VectorF> &beats, int samplesCount);
    static float beatToBeatVariance(QVector<StrainStatistics> &beats, int samplesCount);

    static void segmentStatistics(const QVector<StrainStatistics> &beats, int samplesCount);
    //static VectorF beatToBeatVariancePerSegment(StrainStatistics &firstBeat, StrainStatistics &secondBeat, int samplesCount);
    //static VectorF beatToBeatVariancePerSegment(QVector<StrainStatistics> &beats, int samplesCount);

    static StrainStatistics meanBeatStats(const QVector<StrainStatistics> &beats, int samplesCount);

    static StrainStatistics getOneBeatStats(const VideoDataClip *clip, Strain *strainModel,
                                            int beatIndex, ShapeMap &shapesMap, bool *success);

    static QVector<StrainStatistics> getAllBeatsStats(const VideoDataClip *clip, Strain *strainModel, ShapeMap &shapesMap);
};

#endif // STRAINSTATISTICS_H
