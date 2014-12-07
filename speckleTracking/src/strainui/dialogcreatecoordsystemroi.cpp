#include "dialogcreatecoordsystemroi.h"
#include "ui_dialogcreatecoordsystemroi.h"

#include "uiutils.h"

DialogCreateCoordSystemROI::DialogCreateCoordSystemROI(Mat8 &image, CoordSystemROI *coordSystem, QWidget *parent) :
    DialogCreateCoordSystem(parent), coordSystem(coordSystem), ui(new Ui::DialogCreateCoordSystemROI), loadCompleted(false)
{
    ui->setupUi(this);
    image.copyTo(originalImage);

    ui->gvOriginal->setScene(new QGraphicsScene());
    ui->gvTransformed->setScene(new QGraphicsScene());

    ui->sbX->setValue(coordSystem->roi.x);
    ui->sbY->setValue(coordSystem->roi.y);
    ui->sbWidth->setValue(coordSystem->roi.width);
    ui->sbHeight->setValue(coordSystem->roi.height);

    drawResult(coordSystem);
    loadCompleted = true;
}

DialogCreateCoordSystemROI::~DialogCreateCoordSystemROI()
{
    delete ui;
}

void DialogCreateCoordSystemROI::intValueChanged(int)
{
    recalculate();
}

void DialogCreateCoordSystemROI::recalculate()
{
    if (!loadCompleted) return;

    CoordSystemROI *c = getNewCoordSystem();
    drawResult(c);
    delete c;
}

void DialogCreateCoordSystemROI::drawResult(CoordSystemROI *c)
{
    QGraphicsScene *originalScene = ui->gvOriginal->scene();
    originalScene->clear();

    QPixmap originalPixmap = UIUtils::Mat8ToQPixmap(originalImage);
    originalScene->addPixmap(originalPixmap);
    c->draw(originalScene);

    QGraphicsScene *transformedScene = ui->gvTransformed->scene();
    transformedScene->clear();

    Mat8 transformedImage = c->transform(originalImage);
    QPixmap transformedPixmap = UIUtils::Mat8ToQPixmap(transformedImage);
    transformedScene->addPixmap(transformedPixmap);
}

CoordSystemROI* DialogCreateCoordSystemROI::getNewCoordSystem()
{
    return new CoordSystemROI(cv::Rect(ui->sbX->value(), ui->sbY->value(),
                                       ui->sbWidth->value(), ui->sbHeight->value()));
}
