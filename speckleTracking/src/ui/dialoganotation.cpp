#include "dialoganotation.h"
#include "ui_dialoganotation.h"

DialogAnotation::DialogAnotation(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogAnotation)
{
    ui->setupUi(this);
}

DialogAnotation::~DialogAnotation()
{
    delete ui;
}

void DialogAnotation::setImage(QPixmap pixmap)
{
    ui->widgetAnotation->setImage(pixmap);
}

Points DialogAnotation::getControlPoints()
{
    return ui->widgetAnotation->getControlPoints();
}

void DialogAnotation::setControlPoints(const Points &controlPoints)
{
    ui->widgetAnotation->setControlPoints(controlPoints);
}

int DialogAnotation::getShapeWidth()
{
    return ui->spinBox->value();
}

void DialogAnotation::on_spinBox_valueChanged(int newValue)
{
    ui->widgetAnotation->setShapeWidth(newValue);
}
