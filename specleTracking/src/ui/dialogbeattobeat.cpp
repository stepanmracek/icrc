#include "dialogbeattobeat.h"
#include "ui_dialogbeattobeat.h"

#include <QAction>

#include "linalg/vecf.h"

DialogBeatToBeat::DialogBeatToBeat(Strain *strainModel, QVector<StrainStatistics> &beatsStats, QWidget *parent) :
    QDialog(parent), ui(new Ui::DialogBeatToBeat), strainModel(strainModel), beatsStats(beatsStats)
{
    ui->setupUi(this);
    int segCount = strainModel->segmentsCount;

    ui->comboBox->addItem("Main strain");
    ui->comboBox->addItem("All segments");
    for (int i = 0; i < segCount; i++)
    {
        ui->comboBox->addItem("Segment " + QString::number(i+1));
    }

    on_comboBox_activated("Main strain");
}

DialogBeatToBeat::~DialogBeatToBeat()
{
    delete ui;
}

void DialogBeatToBeat::on_comboBox_activated(const QString &text)
{
    qDebug() << "Displaying" << text;
    ui->plot->detachItems();

    if (text.compare("Main strain") == 0)
    {
        addMainStrain();
    }
    else if (text.compare("All segments") == 0)
    {
        addAllSegments();
    }
    else
    {
        QString strIndex = text.split(" ")[1];
        int index = strIndex.toInt()-1;
        addSegment(index);
    }

    ui->plot->replot();
}

void DialogBeatToBeat::addMainStrain()
{
    for (int i = 0; i < beatsStats.count(); i++)
    {
        const StrainStatistics &beat = beatsStats.at(i);

        VectorF curve = VecF::resample(beat.strain, 100);
        ui->plot->addData(curve, QString::number(i), Qt::white);
    }
}

void DialogBeatToBeat::addAllSegments()
{
    int segCount = strainModel->segmentsCount;
    for (int i = 0; i < beatsStats.count(); i++)
    {
        const StrainStatistics &beat = beatsStats.at(i);

        for (int j = 0; j < segCount; j++)
        {
            VectorF curve = VecF::resample(beat.strainForSegments[j], 100);
            ui->plot->addData(curve, QString::number(i)+"-"+QString::number(j), QColor::fromHsvF((float)j/segCount, 1, 1));
        }
    }
}

void DialogBeatToBeat::addSegment(int index)
{
    int segCount = strainModel->segmentsCount;
    for (int i = 0; i < beatsStats.count(); i++)
    {
        const StrainStatistics &beat = beatsStats.at(i);

        VectorF curve = VecF::resample(beat.strainForSegments[index], 100);
        ui->plot->addData(curve, QString::number(i)+"-"+QString::number(index), QColor::fromHsvF((float)index/segCount, 1, 1));
    }
}
