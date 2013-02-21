#ifndef STRAINRESULTPROCESSING_H
#define STRAINRESULTPROCESSING_H

#include <QObject>

#include "linalg/common.h"
#include "videodatabase.h"

class StrainResultProcessingBase : public QObject
{
    Q_OBJECT

public:
    StrainResultProcessingBase(QObject *parent = 0) : QObject(parent) { }

    virtual VectorOfShapes process(VectorOfShapes &strainResult, VideoDataBase &videoData) = 0;
};

class StrainResultProcessingPass : public StrainResultProcessingBase
{
    Q_OBJECT

public:
    StrainResultProcessingPass(QObject *parent = 0) : StrainResultProcessingBase(parent) { }

    virtual VectorOfShapes process(VectorOfShapes &strainResult, VideoDataBase &) { return strainResult; }
};

class StrainResProcFloatingAvg : public StrainResultProcessingBase
{
    Q_OBJECT

private:
    int windowSize;

public:
    StrainResProcFloatingAvg(int windowSize, QObject *parent = 0) : StrainResultProcessingBase(parent), windowSize(windowSize) { }

    virtual VectorOfShapes process(VectorOfShapes &strainResult, VideoDataBase &videoData);
};

#endif // STRAINRESULTPROCESSING_H
