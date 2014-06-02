#ifndef BATCHSTRAINTRACKER_H
#define BATCHSTRAINTRACKER_H

#include "strain/shapetracker.h"

class BatchStrainTracker
{
public:
    BatchStrainTracker() {}

    void process();

    ShapeTracker *learn();
};

#endif // BATCHSTRAINTRACKER_H
