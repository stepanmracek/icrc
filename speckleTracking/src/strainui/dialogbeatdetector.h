#ifndef DIALOGBEATDETECTOR_H
#define DIALOGBEATDETECTOR_H

#include <QDialog>

#include "strain/beatdetector.h"

namespace Ui {
class DialogBeatDetector;
}

class DialogBeatDetector : public QDialog
{
    Q_OBJECT

    QMap<QString, BeatDetector::DetectionSource> detectionSources;
    QMap<QString, BeatDetector::PrevFramePolicy> prevFramePolicies;

public:
    explicit DialogBeatDetector(QWidget *parent = 0);
    ~DialogBeatDetector();

    BeatDetector::Settings getSettings();

private:
    Ui::DialogBeatDetector *ui;
};

#endif // DIALOGBEATDETECTOR_H
