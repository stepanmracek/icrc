#ifndef BATCHSTRAINTRACKER_H
#define BATCHSTRAINTRACKER_H

#include "strain/shapetracker.h"

class BatchStrainTracker
{
public:
    BatchStrainTracker() {}

    static void extractStrains(QString dirPath);
    static void evaluate(const QString &positives, const QString &negatives);

    static ShapeTracker *learn();

private:
    static QMultiMap<QString, VectorF> loadDirectoryWithStrains(const QString &path);
};

#endif // BATCHSTRAINTRACKER_H
