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
