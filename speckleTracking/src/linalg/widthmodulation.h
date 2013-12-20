#ifndef WIDTHMODULATION_H
#define WIDTHMODULATION_H

#include "common.h"

class WidthModulation
{
public:
    static VectorF generateValues(int count, float frequency, float phase, float baseWidth, float scale);

    static std::vector<VectorF> generateValues(int count, float freqStart, float freqEnd, float freqStep,
                                               float phaseSteps, float baseWidth, float scale);

    static void test();
};

#endif // WIDTHMODULATION_H
