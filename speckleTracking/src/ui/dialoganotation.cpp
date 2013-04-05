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

int DialogAnotation::getShapeWidth()
{
    return ui->spinBox->value();
}