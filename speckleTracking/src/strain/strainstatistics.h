#ifndef STRAINSTATISTICS_H
#define STRAINSTATISTICS_H

#include <list>

#include "linalg/common.h"
#include "strain.h"
#include "videodataclip.h"

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
    StrainStatistics(Strain *strainModel, VectorOfShapes &shapes);

    P getCenter(Points &points);

    static float beatToBeatVariance(StrainStatistics &firstBeat, StrainStatistics &secondBeat, int samplesCount);

    static StrainStatistics getOneBeatStats(VideoDataClip *clip, Strain *strainModel,
                                            int beatIndex, ShapeMap &shapesMap, bool *success);

    static QVector<StrainStatistics> getAllBeatsStats(VideoDataClip *clip, Strain *strainModel, ShapeMap &shapesMap);
};

#endif // STRAINSTATISTICS_H
