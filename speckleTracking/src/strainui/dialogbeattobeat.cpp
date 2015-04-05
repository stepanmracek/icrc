#include "dialogbeattobeat.h"
#include "ui_dialogbeattobeat.h"

#include <QAction>

#include "linalg/vecf.h"

DialogBeatToBeat::DialogBeatToBeat(const QVector<StrainStatistics> &beatsStats, QWidget *parent) :
    QDialog(parent), ui(new Ui::DialogBeatToBeat), beatsStats(beatsStats)
{
    ui->setupUi(this);
    segmentCount = beatsStats.first().strainForSegments.size();
    meanBeatStats = StrainStatistics::meanBeatStats(beatsStats, 100);

    ui->comboBox->addItem("Main strain");
    ui->comboBox->addItem("All segments");
    for (int i = 0; i < segmentCount; i++)
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
    //qDebug() << "Displaying" << text;
    ui->plotBeats->detachItems();
    ui->plotDiff->detachItems();
    ui->plotMean->detachItems();

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

    ui->plotBeats->replot();
    ui->plotDiff->replot();
    ui->plotMean->replot();
}

void DialogBeatToBeat::addMainStrain()
{
    int beatCount = beatsStats.count();
    VectorF resampledBeats[beatCount];
    for (int i = 0; i < beatCount; i++)
    {
        const StrainStatistics &beat = beatsStats.at(i);
        resampledBeats[i] = VecF::resample(beat.strain, 100);

        ui->plotBeats->addData(resampledBeats[i], QString::number(i), Qt::white);
    }

    VectorF diff;
    VectorF mean;
    for (int i = 0; i < 100; i++)
    {
        VectorF slice;
        for (int beat = 0; beat < beatCount; beat++)
        {
            slice.push_back(resampledBeats[beat][i]);
        }

        diff.push_back(VecF::stdDeviation(slice));
        mean.push_back(VecF::meanValue(slice));
    }

    ui->plotDiff->addData(diff, "diff", Qt::white);
    ui->plotMean->addData(mean, "mean", Qt::white);
}

void DialogBeatToBeat::addAllSegments()
{
    for (int i = 0; i < beatsStats.count(); i++)
    {
        const StrainStatistics &beat = beatsStats.at(i);

        for (int j = 0; j < segmentCount; j++)
        {
            VectorF curve = VecF::resample(beat.strainForSegments[j], 100);
            ui->plotBeats->addData(curve, QString::number(i)+"-"+QString::number(j), QColor::fromHsvF((float)j/segmentCount, 1, 1));
        }
    }

    ui->plotMean->addData(meanBeatStats.strain, "mean", Qt::white);
    for (int j = 0; j < segmentCount; j++)
    {
        ui->plotMean->addData(meanBeatStats.strainForSegments[j], "mean-"+QString::number(j), QColor::fromHsvF((float)j/segmentCount, 1, 1));
    }
}

void DialogBeatToBeat::addSegment(int index)
{
    int n = beatsStats.count();
    VectorF resampledBeats[n];
    for (int i = 0; i < n; i++)
    {
        const StrainStatistics &beat = beatsStats.at(i);
        resampledBeats[i] = VecF::resample(beat.strainForSegments[index], 100);

        ui->plotBeats->addData(resampledBeats[i],
                               QString::number(i) + "-" + QString::number(index),
                               QColor::fromHsvF((float)index/segmentCount, 1, 1));
    }

    VectorF diff;
    VectorF mean;
    for (int i = 0; i < 100; i++)
    {
        VectorF slice;
        for (int j = 0; j < n; j++)
        {
            slice.push_back(resampledBeats[j][i]);
        }

        diff.push_back(VecF::stdDeviation(slice));
        mean.push_back(VecF::meanValue(slice));
    }

    ui->plotDiff->addData(diff, "diff", QColor::fromHsvF((float)index/segmentCount, 1, 1));
    ui->plotMean->addData(mean, "mean", QColor::fromHsvF((float)index/segmentCount, 1, 1));
}
