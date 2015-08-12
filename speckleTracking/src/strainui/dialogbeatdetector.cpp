#include "dialogbeatdetector.h"
#include "ui_dialogbeatdetector.h"

DialogBeatDetector::DialogBeatDetector(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogBeatDetector)
{
    ui->setupUi(this);

    detectionSources["Amplitude"] = BeatDetector::DetectionSource::Amplitude;
    detectionSources["Angle"] = BeatDetector::DetectionSource::Angle;
    detectionSources["Center Direction"] = BeatDetector::DetectionSource::CenterDirection;

    prevFramePolicies["First"] = BeatDetector::PrevFramePolicy::First;
    prevFramePolicies["Previous"] = BeatDetector::PrevFramePolicy::Prev;

    ui->cmbSource->addItems(detectionSources.keys());
    ui->cmbPrevFrame->addItems(prevFramePolicies.keys());

    BeatDetector::Settings defaultSettings;
    ui->sbSpacing->setValue(defaultSettings.gridSpacing);
    ui->sbKernelSize->setValue(defaultSettings.averagingKernelSize);
    ui->cmbSource->setCurrentText(detectionSources.key(defaultSettings.detectionSource));
    ui->cmbPrevFrame->setCurrentText(prevFramePolicies.key(defaultSettings.prevFramePolicy));
}

DialogBeatDetector::~DialogBeatDetector()
{
    delete ui;
}

BeatDetector::Settings DialogBeatDetector::getSettings()
{
    return BeatDetector::Settings(
                ui->sbKernelSize->value(),
                ((ui->sbSpacing->value() / 2) * 2) + 1, // to ensure that the spacing will be odd;
                detectionSources[ui->cmbSource->currentText()],
                prevFramePolicies[ui->cmbPrevFrame->currentText()]);
}
