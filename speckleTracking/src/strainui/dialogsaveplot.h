#ifndef DIALOGSAVEPLOT_H
#define DIALOGSAVEPLOT_H

#include <QDialog>

#include "widgetplot.h"

namespace Ui {
class DialogSavePlot;
}

class DialogSavePlot : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogSavePlot(WidgetPlot *plot, QWidget *parent = 0);
    ~DialogSavePlot();
    
private slots:
    void on_buttonBox_accepted();

private:
    Ui::DialogSavePlot *ui;
    WidgetPlot *plot;
};

#endif // DIALOGSAVEPLOT_H
