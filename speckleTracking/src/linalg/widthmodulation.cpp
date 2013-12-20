#include "widthmodulation.h"

VectorF WidthModulation::generateValues(int count, float frequency, float phase, float baseWidth, float scale)
{
    VectorF vec;
    for (int i = 0; i < count; i++)
    {
        float v = baseWidth +  sin(i/(count/frequency)*2.0*M_PI + phase*2*M_PI) * scale * baseWidth;
        vec.push_back(v);
    }
    return vec;
}

std::vector<VectorF> WidthModulation::generateValues(int count, float freqStart, float freqEnd, float freqStep,
                                                     float phaseSteps, float baseWidth, float scale)
{
    std::vector<VectorF> result;
    for (float freq = freqStart; freq <= freqEnd; freq += freqStep)
    {
        for (float phase = 0; phase <= 1.0; phase += 1.0/phaseSteps)
        {
            //qDebug() << "freq" << freq << "phase" << phase;
            VectorF vec = generateValues(count, freq, phase, baseWidth, scale);
            result.push_back(vec);
        }
    }

    return result;
}

struct WidthModulationStruct
{
    WidthModulationStruct()
        : freq(0), phase(0), scale(10)
    { }

    VectorF modulationValues()
    {
        float f = (freq + 1.0) / 2.0;
        float p = phase / 10.0;
        float s = scale / 10.0;
        return WidthModulation::generateValues(640, f, p, 100, s);
    }

    int freq;
    int phase;
    int scale;
};

void show(WidthModulationStruct *data)
{
    Mat8 frame = Mat8::zeros(200, 640);

    VectorF values = data->modulationValues();
    for (int i = 1; i < 640; i++)
    {
        float v = 200-values[i-1];
        int y1 = v;
        int x1 = (i-1);

        v = 200-values[i];
        int y2 = v;
        int x2 = i;

        cv::line(frame, cv::Point(x1, y1), cv::Point(x2, y2), 255);

        /*v = -diff[i-1];
        y1 = 200-(v + 1.0)*100;
        x1 = (i-1)*10;

        v = -diff[i];
        y2 = 200-(v + 1.0)*100;
        x2 = i*10;*/

        //cv::line(frame, cv::Point(x1, y1), cv::Point(x2, y2), 192);

        /*v = diff2[i-1];
        y1 = 200-(v + 1.0)*100;
        x1 = (i-1)*10;

        v = diff2[i];
        y2 = 200-(v + 1.0)*100;
        x2 = i*10;

        cv::line(frame, cv::Point(x1, y1), cv::Point(x2, y2), 127);*/
    }

    /*Points points = FrequencyModulation::modulate(data->points, values);
    for (int i = 0; i < n; i++)
    {
        cv::circle(frame, points[i], 2, 255);
    }*/

    cv::imshow("modulation", frame);
}

void onChangeWidthModulation(int /*pos*/, void *data)
{
    show((WidthModulationStruct*) data);
}

void WidthModulation::test()
{
    cv::namedWindow("modulation");

    WidthModulationStruct data;
    cv::createTrackbar("freq", "modulation", &data.freq, 10, onChangeWidthModulation, &data);
    cv::createTrackbar("phase", "modulation", &data.phase, 10, onChangeWidthModulation, &data);
    cv::createTrackbar("scale", "modulation", &data.scale, 10, onChangeWidthModulation, &data);
    show(&data);
    while ((char)cv::waitKey() != 27) {}
}
