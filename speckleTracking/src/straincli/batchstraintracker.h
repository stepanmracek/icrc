#ifndef BATCHSTRAINTRACKER_H
#define BATCHSTRAINTRACKER_H

#include "strain/shapetracker.h"
#include "strain/strainstatistics.h"

class BatchStrainTracker
{
public:
    BatchStrainTracker() {}

    static void extractStrains(const QString &inputDirPath, const QString &outputDirPath);
    static void stats(const QString &extractedStainsDirPath);

    static void evaluate(const QString &positives, const QString &negatives);
    static ShapeTracker *learn();

private:
    static QMultiMap<QString, VectorF> loadDirectoryWithStrains(const QString &path);
    static QMultiMap<QString, StrainStatistics> loadDirectoryWithStrains(const QString &path, const QString &statsFileSuffix);
};

#endif // BATCHSTRAINTRACKER_H
