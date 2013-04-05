#include "widgetplot.h"

#include <cfloat>

WidgetPlot::WidgetPlot(QWidget *parent) : QwtPlot(parent)
{
    indexCurve = NULL;
    setCanvasBackground(QBrush(Qt::black));
    dataLoaded = false;
    minValue = FLT_MAX;
    maxValue = -FLT_MAX;
}

void WidgetPlot::addData(VectorF &data, const QString &name, const QColor &color)
{
    QwtPlotCurve *curve = new QwtPlotCurve(name);
    QVector<QPointF> curveData;
    for (size_t i = 0; i < data.size(); i++)
    {
        float value = data[i];
        if (value > maxValue) maxValue = value;
        if (value < minValue) minValue = value;
        QPointF p(i, value);
        curveData << p;
    }

    curve->setRenderHint(QwtPlotItem::RenderAntialiased);
    curve->setSamples(curveData);
    curve->setPen(QPen(color));
    curve->attach(this);

    dataLoaded = true;
}

void WidgetPlot::setIndex(int index)
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
        indexCurve->setPen(QPen(Qt::white));
    }

    QVector<QPointF> data;
    data << QPointF(index, minValue);
    data << QPointF(index, maxValue);
    indexCurve->setSamples(data);
    indexCurve->attach(this);
    this->setAxisScale(QwtPlot::yLeft, minValue,  maxValue);
    this->replot();
}
