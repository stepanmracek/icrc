#include "widgetspectrogram.h"

#include <qwt/qwt_plot_spectrogram.h>
#include <qwt/qwt_matrix_raster_data.h>
#include <qwt/qwt_color_map.h>
#include <cassert>

WidgetSpectrogram::WidgetSpectrogram(QWidget *parent) : QwtPlot(parent)
{
    dataLoaded = false;
    setCanvasBackground(QBrush(Qt::black));

    indexCurve = NULL;

    spectrogram = new QwtPlotSpectrogram("spectrogram");
    QwtLinearColorMap *colorMap = new QwtLinearColorMap(Qt::red, Qt::blue);
    colorMap->addColorStop(0.5, Qt::white);
    spectrogram->setColorMap(colorMap);
    spectrogram->attach(this);
}

void WidgetSpectrogram::setData(std::vector<VectorF> &rawData)
{
    int rows = rawData.size();
    assert(rows > 0);

    int cols = rawData[0].size();
    assert(cols > 0);

    QVector<double> valueMatrix; //(rows*cols);

    for (int r = 0; r < rows; r++)
    {
        double min = 1e300;
        double max = -1e300;
        for (int c = 0; c < cols; c++)
        {
            double val = rawData[r][c];
            if (val > max) max = val;
            if (val < min) min = val;
        }
        for (int c = 0; c < cols; c++)
        {
            double val = rawData[r][c];
            valueMatrix << (val-min)/(max-min);
        }
    }

    QwtMatrixRasterData *data = new QwtMatrixRasterData();
    data->setValueMatrix(valueMatrix, cols);
    data->setInterval(Qt::XAxis, QwtInterval(0, cols));
    data->setInterval(Qt::YAxis, QwtInterval(0, 1));
    data->setInterval(Qt::ZAxis, QwtInterval(0, 1));
    data->setResampleMode(QwtMatrixRasterData::BilinearInterpolation);
    spectrogram->setData(data);
    dataLoaded = true;
}

void WidgetSpectrogram::setIndex(int index)
{
    if (!dataLoaded) return;

    if (indexCurve != NULL)
    {
        indexCurve->detach();
    }
    else
    {
        indexCurve = new QwtPlotCurve("index");
        //indexCurve->setRenderHint(QwtPlotItem::RenderAntialiased);
        indexCurve->setPen(QPen(Qt::black));
    }

    QVector<QPointF> data;
    data << QPointF(index, 0);
    data << QPointF(index, 1);
    indexCurve->setSamples(data);
    indexCurve->attach(this);
    this->setAxisScale(QwtPlot::yLeft, 0,  1);
    this->replot();
}
