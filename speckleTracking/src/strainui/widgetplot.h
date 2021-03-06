#ifndef WIDGETPLOT_H
#define WIDGETPLOT_H

#include <QMouseEvent>
#include <qwt/qwt_plot.h>
#include <qwt/qwt_plot_curve.h>
#include <vector>

#include "strain/strainstatistics.h"

class WidgetPlot : public QwtPlot
{
    Q_OBJECT

    QwtPlotCurve *indexCurve;
    float minValue;
    float maxValue;
    bool dataLoaded;

public:
    //StrainStatistics &statistics;

    WidgetPlot(QWidget *parent = 0);

    void addData(VectorF &data, const QString &name, const QColor &color);

protected:
    void mousePressEvent(QMouseEvent *event);
    
public slots:
    void setIndex(int index);
    void saveCurves();
};

#endif // WIDGETPLOT_H
