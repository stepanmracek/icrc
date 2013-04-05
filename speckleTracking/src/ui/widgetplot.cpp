#include "widgetplot.h"

#include <QMenu>
#include <QFileDialog>
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

void WidgetPlot::saveCurves()
{
    QString selectedDir = QFileDialog::getExistingDirectory(this);
    if (selectedDir.isNull() || selectedDir.isEmpty()) return;

    QwtPlotItemList list = this->itemList();
    QFile plotFile(selectedDir + QDir::separator() + "plot.p");
    plotFile.open(QIODevice::WriteOnly);
    QTextStream plotStream(&plotFile);
    plotStream << "set terminal postscript enhanced color\nset output '| ps2pdf - plot.pdf'\n";
    plotStream << "plot ";

    foreach(const QwtPlotItem *item, list)
    {
        const QString &name = item->title().text();
        if (name.compare("index") == 0) continue;

        QwtPlotCurve *curve = (QwtPlotCurve*)(item);
        if (curve == 0) continue;

        QFile f(selectedDir + QDir::separator() + name);
        f.open(QIODevice::WriteOnly);
        QTextStream out(&f);

        int n = curve->dataSize();
        for (int i = 0; i < n; i++)
        {
            const QPointF &p = curve->data()->sample(i);
            out << p.x() << ' ' << p.y() << '\n';
        }
        out.flush();
        f.close();

        plotStream << '\"' + name + '\"' + " w l,";
    }
    plotStream.flush();
    plotFile.close();
}

void WidgetPlot::mousePressEvent(QMouseEvent *event)
{
    QList<QAction*> actions;
    QAction *actionSave = new QAction("Save curves", this);
    actionSave->connect(actionSave, SIGNAL(triggered()), this, SLOT(saveCurves()));
    actions.append(actionSave);
    if (event->button() == Qt::RightButton)
    {
        QMenu::exec(actions, this->mapToGlobal(event->pos()));
    }
}
