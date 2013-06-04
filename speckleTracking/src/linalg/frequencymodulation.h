#ifndef FREQUENCYMODULATION_H
#define FREQUENCYMODULATION_H

#include "common.h"
class FrequencyModulation
{
public:    
    static Points modulate(const Points &points, const VectorF &modulationValues);

    static VectorF generateModulationValues(int resultLen, float freq, float phase, float scale);

    static std::vector<VectorF> generateModulationValues(int resultLen, float freqStart, float freqEnd,
                                                         float freqStep, float phaseSteps, float scale);

    static void test();
};

#endif // FREQUENCYMODULATION_H
