#include "dialogbeattobeat.h"
#include "ui_dialogbeattobeat.h"

#include "linalg/vecf.h"

DialogBeatToBeat::DialogBeatToBeat(QVector<StrainStatistics> &beatsStats, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogBeatToBeat)
{
    ui->setupUi(this);

    for (int i = 0; i < beatsStats.count(); i++)
    //foreach(const StrainStatistics &beat, beatsStats)
    {
        const StrainStatistics &beat = beatsStats.at(i);
        VectorF beatStrain = VecF::resample(beat.strain, 100);
        ui->plot->addData(beatStrain, QString::number(i), Qt::white );
    }
}

DialogBeatToBeat::~DialogBeatToBeat()
{
    delete ui;
}
