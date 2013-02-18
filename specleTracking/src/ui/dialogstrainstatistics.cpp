#include "dialogstrainstatistics.h"
#include "ui_dialogstrainstatistics.h"

DialogStrainStatistics::DialogStrainStatistics(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogStrainStatistics)
{
    ui->setupUi(this);
}

DialogStrainStatistics::~DialogStrainStatistics()
{
    delete ui;
}

void DialogStrainStatistics::SetData(StrainStatistics *strainStatistics, ShapeTracker *tracker, VideoDataClip &clip, QMap<int, Points> &shapes)
{
    ui->mainStrainPlot->addData(strainStatistics->strain, "Average strain", Qt::white);
    ui->mainStrainRatePlot->addData(strainStatistics->strainRate, "Average strain rate", Qt::white);

    int pointCount = strainStatistics->strainForSegments.size();
    for (int i = 0; i < pointCount; i++)
    {
        double hue = ((double)i)/pointCount;
        QColor color = QColor::fromHsvF(hue, 1, 1);
        QString pointPlotName = QString::number(i+1);

        ui->strainPerPointsPlot->addData(strainStatistics->strainForSegments[i], pointPlotName, color);
        ui->strainRatePerPointsPlot->addData(strainStatistics->strainRateForSegments[i], pointPlotName, color);
    }

    ui->spectrogramPlot->setData(strainStatistics->strainForSegments);
    ui->spectrogramRatePlot->setData(strainStatistics->strainRateForSegments);

    ui->strainVideoWidget->setTracker(tracker);
    ui->strainVideoWidget->setClip(clip);
    ui->strainVideoWidget->shapes = shapes;
    ui->strainVideoWidget->display(0);
}
