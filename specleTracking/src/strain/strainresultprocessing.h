#ifndef STRAINRESULTPROCESSING_H
#define STRAINRESULTPROCESSING_H

#include "linalg/common.h"
#include "videodatabase.h"

class StrainResultProcessingBase
{
public:
    virtual VectorOfShapes process(VectorOfShapes &strainResult, VideoDataBase &videoData) = 0;
};

class StrainResultProcessingPass : public StrainResultProcessingBase
{
public:
    virtual VectorOfShapes process(VectorOfShapes &strainResult, VideoDataBase &) { return strainResult; }
};

class StrainResProcFloatingAvg : public StrainResultProcessingBase
{
private:
    int windowSize;

public:
    StrainResProcFloatingAvg(int windowSize) : windowSize(windowSize) {}

    virtual VectorOfShapes process(VectorOfShapes &strainResult, VideoDataBase &videoData);
};

#endif // STRAINRESULTPROCESSING_H
