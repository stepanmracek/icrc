#include "graphicsviewwithevents.h"

#include <QMouseEvent>

GraphicsViewWithEvents::GraphicsViewWithEvents(QWidget *parent) :
    QGraphicsView(parent)
{
}

void GraphicsViewWithEvents::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        QPointF scenePoint = mapToScene(event->pos());
        double x = scenePoint.x();
        double y = scenePoint.y();
        emit leftMouseClick(QPoint(x, y));
    }
}
