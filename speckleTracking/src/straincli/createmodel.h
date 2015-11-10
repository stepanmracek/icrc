#ifndef CREATEMODEL_H
#define CREATEMODEL_H

#include <QString>
#include <QCommandLineParser>
#include "linalg/common.h"

class CreateModel
{
public:
    
    struct Settings
    {
        float freqStart;
        float freqEnd;
        float freqStep;
        float phaseSteps;
        float amplitude;
        int pointsPerSegment;
        int segments;
        VectorF widths;
        
        Settings()
        {
            freqStart = 1.0;
            freqEnd = 3.0;
            freqStep = 1.0;
            phaseSteps = 3.0;
            amplitude = 1.0;
            pointsPerSegment = 10;
            segments = 6;
            widths.push_back(15); widths.push_back(20); widths.push_back(25);
        }
        
        static QList<QCommandLineOption> getCommandLineOptions();
        static Settings get(const QCommandLineParser &parser);
        void print() const;
    };
    
    static void create(const QString &dirPath, const QString &modelName, const Settings &settings = Settings());
};

#endif // CREATEMODEL_H
