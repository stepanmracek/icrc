#include "widgetresult.h"

WidgetResult::WidgetResult(QWidget *parent) :
    QGraphicsView(parent)
{
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


void WidgetResult::setControlPoints(Points controlPoints, int shapeWidth)
{
    if (tracker == NULL)
    {
        emit trackerNotSet();
        return;
    }
    if (!imageSet)
    {
        emit imageNotSet();
        return;
    }

    setResultPoints(tracker->strain.getRealShapePoints(controlPoints, shapeWidth));
}

void WidgetResult::setResultPoints(Points points)
{
    if (tracker == NULL)
    {
        emit trackerNotSet();
        return;
    }
    if (!imageSet)
    {
        emit imageNotSet();
        return;
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

    graphicItems = tracker->strain.drawResult(scene, points);
    coordGraphicItems = tracker->coordSystem->draw(scene);
}

