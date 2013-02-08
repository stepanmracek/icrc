#ifndef STRAINSTATISTICS_H
#define STRAINSTATISTICS_H

#include <list>

#include "linalg/common.h"
#include "strain.h"

class StrainStatistics
{
public:
    VectorF strain;
    VectorF strainRate;

    std::vector<VectorF> strainForSegments;
    std::vector<VectorF> strainRateForSegments;

    StrainStatistics(Strain &strainModel, VectorOfShapes &shapes);

    P getCenter(Points &points);

};

#endif // STRAINSTATISTICS_H
