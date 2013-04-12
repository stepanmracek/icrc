#include "dialogsaveplot.h"
#include "ui_dialogsaveplot.h"

DialogSavePlot::DialogSavePlot(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogSavePlot)
{
    ui->setupUi(this);
}

DialogSavePlot::~DialogSavePlot()
{
    delete ui;
}
