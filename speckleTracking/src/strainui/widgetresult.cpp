#include "widgetresult.h"

WidgetResult::WidgetResult(QWidget *parent) :
    QGraphicsView(parent)
{
    this->setRenderHints(QPainter::Antialiasing);

    tracker = NULL;
    imageSet = false;
    scene = new QGraphicsScene();
    this->setScene(scene);
    scene->setBackgroundBrush(QBrush(Qt::black));
}

void WidgetResult::setImage(QPixmap pixmap)
{
    graphicItems.clear();
    coordGraphicItems.clear();
    resultPoints.clear();
    scene->clear();
    scene->addPixmap(pixmap);
    imageSet = true;
}


void WidgetResult::setControlPoints(Points controlPoints, int shapeWidth, CoordSystemBase *coordSystem)
{
    if (tracker == NULL)
    {
        emit trackerNotSet();
        return;
    }
    if (!imageSet)
    {
        emit imageNotSet();
        //return;
    }

    setResultPoints(tracker->getStrain()->getRealShapePoints(controlPoints, shapeWidth), coordSystem);
}

void WidgetResult::setResultPoints(Points points, CoordSystemBase *coordSystem)
{
    if (tracker == NULL)
    {
        emit trackerNotSet();
        return;
    }
    if (!imageSet)
    {
        emit imageNotSet();
        //return;
    }

    resultPoints = points;
    foreach(QGraphicsItem *i, graphicItems)
    {
        scene->removeItem(i);
    }
    foreach(QGraphicsItem *i, coordGraphicItems)
    {
        scene->removeItem(i);
    }

    graphicItems = tracker->getStrain()->drawResult(scene, points);

    if (coordSystem != 0)
    {
        coordGraphicItems = coordSystem->draw(scene);
    }
}

