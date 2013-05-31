#include "frequencymodulation.h"

#include <QDebug>

Points FrequencyModulation::modulate(const Points &points, const VectorF &modulationValues)
{
    Points result;
    int n = points.size();
    assert(n == modulationValues.size());
    assert(n >= 3);

    for (int i = 0; i < n; i++)
    {
        float m = modulationValues[i];
        assert(fabs(m) <= 1);
        const P &p = points[i];

        if (m > 0)
        {
            const P next = (i == (n - 1)) ?
                        points[n-1] + (points[n-1] - points[n-2]) :
                        points[i+1];
            P toNext = (next - p)*m*0.5;
            result.push_back(p + toNext);
        }
        else if (m < 0)
        {
            const P prev = (i == 0) ?
                        points[0] + (points[0] - points[1]) :
                        points[i-1];
            P toPrev = (prev - p)*(-m)*0.5;
            result.push_back(p + toPrev);
        }
        else
        {
            result.push_back(p);
        }
    }

    return result;
}

std::vector<VectorF> FrequencyModulation::generateModulationValues(float resultLen, float freqStart, float freqEnd,
                                                                   float freqStep, float phaseSteps, float scale)
{
    std::vector<VectorF> result;

    for (float freq = freqStart; freq <= freqEnd; freq += freqStep)
    {
        for (float phase = 0; phase <= 1.0; phase += 1.0/phaseSteps)
        {
            qDebug() << "freq" << freq << "phase" << phase;
            VectorF vec;
            for (int i = 0; i < resultLen; i++)
            {
                float v = sin(i/(resultLen/freq)*2.0*M_PI + phase*2*M_PI);
                v *= scale;
                //qDebug() << " " << v;
                vec.push_back(v);
            }
            result.push_back(vec);
        }
    }

    return result;
}
