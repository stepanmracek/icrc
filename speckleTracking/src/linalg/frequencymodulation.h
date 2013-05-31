#ifndef FREQUENCYMODULATION_H
#define FREQUENCYMODULATION_H

#include "common.h"
class FrequencyModulation
{
public:    
    static Points modulate(const Points &points, const VectorF &modulationValues);

    static std::vector<VectorF> generateModulationValues(float resultLen, float freqStart, float freqEnd,
                                                         float freqStep, float phaseSteps, float scale);
};

#endif // FREQUENCYMODULATION_H
