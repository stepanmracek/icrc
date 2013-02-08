#ifndef WIDGETSPECTROGRAM_H
#define WIDGETSPECTROGRAM_H

#include <qwt/qwt_plot.h>
#include <qwt/qwt_plot_spectrogram.h>
#include <qwt/qwt_plot_curve.h>

#include "linalg/common.h"

class WidgetSpectrogram : public QwtPlot
{
    Q_OBJECT

    bool dataLoaded;
    QwtPlotSpectrogram *spectrogram;
    QwtPlotCurve *indexCurve;

public:
    WidgetSpectrogram(QWidget *parent);

    void setData(std::vector<VectorF> &rawData);

public slots:
    void setIndex(int index);
};

#endif // WIDGETSPECTROGRAM_H
