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

VectorF FrequencyModulation::generateModulationValues(int resultLen, float freq, float phase, float scale)
{
    VectorF vec;
    for (int i = 0; i < resultLen; i++)
    {
        float v = sin(i/(resultLen/freq)*2.0*M_PI + phase*2*M_PI) * scale;
        vec.push_back(v);
    }
    return vec;
}

std::vector<VectorF> FrequencyModulation::generateModulationValues(int resultLen, float freqStart, float freqEnd,
                                                                   float freqStep, float phaseSteps, float scale)
{
    std::vector<VectorF> result;
    for (float freq = freqStart; freq <= freqEnd; freq += freqStep)
    {
        for (float phase = 0; phase <= 1.0; phase += 1.0/phaseSteps)
        {
            //qDebug() << "freq" << freq << "phase" << phase;
            VectorF vec = generateModulationValues(resultLen, freq, phase, scale);
            result.push_back(vec);
        }
    }

    return result;
}

struct FreqModulationStruct
{
    FreqModulationStruct(const Points &points)
        : points(points), freq(0), phase(0), scale(10)
    { }

    VectorF modulationValues()
    {
        float f = (freq + 1.0) / 2.0;
        float p = phase / 10.0;
        float s = scale / 10.0;
        return FrequencyModulation::generateModulationValues(points.size(), f, p, s);
    }

    const Points &points;
    int freq;
    int phase;
    int scale;
};

void show(FreqModulationStruct *data)
{
    int n = data->points.size();
    Mat8 frame = Mat8::zeros(200, n*10);

    VectorF values = data->modulationValues();
    VectorF diff = Common::deltas(values);
    //VectorF diff2 = Common::deltas(diff);
    for (int i = 1; i < n; i++)
    {
        float v = values[i-1];
        int y1 = 200-(v + 1.0)*100;
        int x1 = (i-1)*10;

        v = values[i];
        int y2 = 200-(v + 1.0)*100;
        int x2 = i*10;

        cv::line(frame, cv::Point(x1, y1), cv::Point(x2, y2), 255);

        v = -diff[i-1];
        y1 = 200-(v + 1.0)*100;
        x1 = (i-1)*10;

        v = -diff[i];
        y2 = 200-(v + 1.0)*100;
        x2 = i*10;

        //cv::line(frame, cv::Point(x1, y1), cv::Point(x2, y2), 192);

        /*v = diff2[i-1];
        y1 = 200-(v + 1.0)*100;
        x1 = (i-1)*10;

        v = diff2[i];
        y2 = 200-(v + 1.0)*100;
        x2 = i*10;

        cv::line(frame, cv::Point(x1, y1), cv::Point(x2, y2), 127);*/
    }

    Points points = FrequencyModulation::modulate(data->points, values);
    for (int i = 0; i < n; i++)
    {
        cv::circle(frame, points[i], 2, 255);
    }

    cv::imshow("modulation", frame);
}

void onChange(int pos, void *data)
{
    show((FreqModulationStruct*) data);
}

void FrequencyModulation::test()
{
    Points points;
    for (int i = 0; i < 60; i++)
    {
        points.push_back(P(i*10, 100));
    }

    cv::namedWindow("modulation");

    FreqModulationStruct data(points);
    cv::createTrackbar("freq", "modulation", &data.freq, 10, onChange, &data);
    cv::createTrackbar("phase", "modulation", &data.phase, 10, onChange, &data);
    cv::createTrackbar("scale", "modulation", &data.scale, 10, onChange, &data);
    show(&data);
    while ((char)cv::waitKey() != 27) {}
}
