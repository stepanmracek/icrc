#include "dialogcreatecoordsystemradial.h"
#include "ui_dialogcreatecoordsystemradial.h"

#include <QDebug>
#include <QMessageBox>
#include <QMouseEvent>

#include "uiutils.h"

DialogCreateCoordSystemRadial::DialogCreateCoordSystemRadial(Mat8 &image, CoordSystemRadial *coordSystem, QWidget *parent):
    DialogCreateCoordSystem(parent), coordSystem(coordSystem), ui(new Ui::DialogCreatCoordSystemRadial), loadCompleted(false)
{
    image.copyTo(originalImage);

    ui->setupUi(this);
    ui->gvOriginal->setScene(new QGraphicsScene());
    ui->gvTransformed->setScene(new QGraphicsScene());
    //ui->gvOriginal->installEventFilter(new MouseFilter);

    ui->dsbCenterX->setValue(coordSystem->center.x);
    ui->dsbCenterY->setValue(coordSystem->center.y);

    ui->dsbAngleStart->setValue(coordSystem->angleStart);
    ui->dsbAngleEnd->setValue(coordSystem->angleEnd);

    ui->dsbDistanceStart->setValue(coordSystem->startDistance);
    ui->dsbDistanceEnd->setValue(coordSystem->endDistance);

    ui->sbResultRows->setValue(coordSystem->resultMatRows);
    ui->sbResultCols->setValue(coordSystem->resultMatCols);

    drawResult(coordSystem);
    loadCompleted = true;
}

DialogCreateCoordSystemRadial::~DialogCreateCoordSystemRadial()
{
    delete ui;
}

/*void DialogCreateCoordSystemRadial::mouseReleaseEvent(QMouseEvent *event)
{
    if (!imageLoaded)
    {
        return;
    }

    if (event->button() == Qt::LeftButton)
    {
        QPointF scenePoint = mapToScene(event->pos());
        float x = scenePoint.x();
        float y = scenePoint.y();

        qDebug() << "CLICK" << x << y;

    }
}*/


void DialogCreateCoordSystemRadial::drawResult(CoordSystemRadial *c)
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

void DialogCreateCoordSystemRadial::doubleValueChanged(double /*arg1*/)
{
    recalculate();
}

void DialogCreateCoordSystemRadial::intValueChanged(int /*arg1*/)
{
    recalculate();
}

bool DialogCreateCoordSystemRadial::check()
{
    /*float centerX = ui->dsbCenterX->value();
    float centerY = ui->dsbCenterY->value();*/
    float angleStart = ui->dsbAngleStart->value();
    float angleEnd = ui->dsbAngleEnd->value();
    float distanceStart = ui->dsbDistanceStart->value();
    float distanceEnd = ui->dsbDistanceEnd->value();
    /*int rows = ui->sbResultRows->value();
    int cols = ui->sbResultCols->value();*/

    if (angleStart >= angleEnd) return false;
    if (distanceStart >= distanceEnd) return false;
    return true;
}

void DialogCreateCoordSystemRadial::recalculate()
{
    if (!loadCompleted) return;

    if (!check())
    {
        QMessageBox info(QMessageBox::Warning, "Non-valid parameters", "Non-valid parameters", QMessageBox::Ok);
        info.exec();
        return;
    }

    CoordSystemRadial *c = getNewCoordSystem();
    drawResult(c);
    delete c;
}

void DialogCreateCoordSystemRadial::on_buttonBox_accepted()
{
    if (!check())
    {
        QMessageBox info(QMessageBox::Warning, "Non-valid parameters", "Non-valid parameters", QMessageBox::Ok);
        info.exec();
        return;
    }

    accept();
}

CoordSystemRadial *DialogCreateCoordSystemRadial::getNewCoordSystem()
{
    float centerX = ui->dsbCenterX->value();
    float centerY = ui->dsbCenterY->value();
    float angleStart = ui->dsbAngleStart->value();
    float angleEnd = ui->dsbAngleEnd->value();
    float distanceStart = ui->dsbDistanceStart->value();
    float distanceEnd = ui->dsbDistanceEnd->value();
    int rows = ui->sbResultRows->value();
    int cols = ui->sbResultCols->value();

    return new CoordSystemRadial(P(centerX, centerY), distanceStart, distanceEnd, angleStart, angleEnd, cols, rows);
}

void DialogCreateCoordSystemRadial::on_gvOriginal_leftMouseClick(const QPoint &point)
{
    ui->dsbCenterX->setValue(point.x());
    ui->dsbCenterY->setValue(point.y());
}
