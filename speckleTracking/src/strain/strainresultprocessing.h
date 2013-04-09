#ifndef STRAINRESULTPROCESSING_H
#define STRAINRESULTPROCESSING_H

#include <QObject>

#include "linalg/common.h"
#include "videodataclip.h"

class StrainResultProcessingBase : public QObject
{
    Q_OBJECT

public:
    StrainResultProcessingBase(QObject *parent = 0) : QObject(parent) { }

    virtual ShapeMap process(ShapeMap &strainResult, int startIndex, int endIndex, VideoDataClip *videoData) = 0;

    virtual QString getInfo() = 0;
};

class StrainResultProcessingPass : public StrainResultProcessingBase
{
    Q_OBJECT

public:
    StrainResultProcessingPass(QObject *parent = 0) : StrainResultProcessingBase(parent) { }

    virtual ShapeMap process(ShapeMap &strainResult, int , int , VideoDataClip *) { return strainResult; }

    QString getInfo() { return "No result processing\n"; }
};

class StrainResProcFloatingAvg : public StrainResultProcessingBase
{
    Q_OBJECT

private:
    int kernelSize;

public:
    StrainResProcFloatingAvg(int kernelSize, QObject *parent = 0);

    virtual ShapeMap process(ShapeMap &strainResult, int startIndex, int endIndex, VideoDataClip *);

    QString getInfo() { return QString("Result processing:\n  floating average window: %1\n").arg(kernelSize); }
};

#endif // STRAINRESULTPROCESSING_H
