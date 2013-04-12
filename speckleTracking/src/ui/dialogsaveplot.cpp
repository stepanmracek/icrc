#include "dialogsaveplot.h"
#include "ui_dialogsaveplot.h"

#include <QString>
#include <QFile>
#include <QTextStream>
#include <QDir>
#include <QFileDialog>
#include <qwt/qwt.h>
#include <qwt/qwt_plot_item.h>
#include <qwt/qwt_plot.h>

DialogSavePlot::DialogSavePlot(WidgetPlot *plot, QWidget *parent) :
    plot(plot), QDialog(parent),
    ui(new Ui::DialogSavePlot)
{
    ui->setupUi(this);
}

DialogSavePlot::~DialogSavePlot()
{
    delete ui;
}

void DialogSavePlot::on_buttonBox_accepted()
{
    QString selectedDir = QFileDialog::getExistingDirectory(this);
    if (selectedDir.isNull() || selectedDir.isEmpty()) return;

    QwtPlotItemList list = plot->itemList();
    QFile plotFile(selectedDir + QDir::separator() + "plot.p");
    plotFile.open(QIODevice::WriteOnly);
    QTextStream plotStream(&plotFile);
    plotStream << "set terminal postscript enhanced color\nset output '| ps2pdf - plot.pdf'\n";

    if (!ui->cbxShowLegend->isChecked())
    {
        plotStream << "set nokey\n";
    }

    QString horizontalAxis = ui->leHorizontalAxis->text();
    if (!horizontalAxis.isNull() && !horizontalAxis.isEmpty())
    {
        plotStream << "set xlabel \"" + horizontalAxis + "\"\n";
    }

    QString verticalAxis = ui->leVerticakAxis->text();
    if (!verticalAxis.isNull() && !verticalAxis.isEmpty())
    {
        plotStream << "set ylabel \"" + verticalAxis + "\"\n";
    }

    QString color;
    if (ui->cbxSameColor->isChecked())
    {
        color = " lt 1";
    }

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

        plotStream << '\"' + name + '\"' + " w l" + color + ",";
    }
    plotStream.flush();
    plotFile.close();
}
