#ifndef BEATDETECTOR_H
#define BEATDETECTOR_H

#include <QObject>

#include "videodataclip.h"

class BeatDetector : public QObject
{
    Q_OBJECT
public:
    enum class DetectionSource { Angle, Amplitude, CenterDirection };
    enum class PrevFramePolicy { First, Prev };

    struct Settings {
        int gridSpacing;
        int averagingKernelSize;
        DetectionSource detectionSource;
        PrevFramePolicy prevFramePolicy;

        Settings(int gridSpacing = 20, int averagingKernelSize = 15,
                 DetectionSource detectionSource = DetectionSource::CenterDirection,
                 PrevFramePolicy prevFramePolicy = PrevFramePolicy::Prev) :
            gridSpacing(gridSpacing), averagingKernelSize(averagingKernelSize),
            detectionSource(detectionSource), prevFramePolicy(prevFramePolicy)
        {}
    };

private:
    Settings settings;

public:
    explicit BeatDetector(const Settings &settings = Settings(), QObject *parent = 0);

    QVector<int> detect(VideoDataClip *clip, bool debug = false, QProgressDialog *progress = 0);
};

#endif // BEATDETECTOR_H
