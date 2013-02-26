#include "dialogimageprocessing.h"
#include "ui_dialogimageprocessing.h"

DialogImageProcessing::DialogImageProcessing(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogImageProcessing)
{
    ui->setupUi(this);
}

DialogImageProcessing::~DialogImageProcessing()
{
    delete ui;
}
