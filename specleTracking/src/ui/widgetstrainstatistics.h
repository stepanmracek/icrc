#ifndef WIDGETSTRAINSTATISTICS_H
#define WIDGETSTRAINSTATISTICS_H

#include <QWidget>
#include <QMap>

#include "strain/strainstatistics.h"
#include "strain/videodatabase.h"
#include "strain/videodataclip.h"
#include "strain/shapetracker.h"

namespace Ui {
class WidgetStrainStatistics;
}

class WidgetStrainStatistics : public QWidget
{
    Q_OBJECT
    
public:
    explicit WidgetStrainStatistics(QWidget *parent = 0);
    ~WidgetStrainStatistics();

    void SetData(StrainStatistics *strainStatistics, ShapeTracker *tracker, VideoDataClip &clip, QMap<int, Points> &shapes);
    
private:
    Ui::WidgetStrainStatistics *ui;
};

#endif // WIDGETSTRAINSTATISTICS_H
