#include "widgetanotation.h"

#include <QDebug>
#include <QMouseEvent>
#include <QColor>

WidgetAnotation::WidgetAnotation(QWidget *parent) :
    QGraphicsView(parent)
{
    whitePen = QPen(Qt::white);
    yellowPen = QPen(Qt::yellow);

    draggingAll = false;
    imageLoaded = false;
    draggingIndex = -1;
    scene = new QGraphicsScene();
    this->setScene(scene);
    setCursor(QCursor(Qt::PointingHandCursor));
    scene->setBackgroundBrush(QBrush(Qt::black));
    //setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    //setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
}

void WidgetAnotation::setImage(QPixmap pixmap)
{
    splineControlPointsCircles.clear();
    controlPoints.clear();
    splineLines.clear();
    scene->clear();
    scene->addPixmap(pixmap);
    imageLoaded = true;
}

void WidgetAnotation::setControlPoints(Points points)
{
    clearControlCircles();
    clearLines();

    controlPoints = points;

    drawLines();
    drawControlCircles();
}

Points WidgetAnotation::getControlPoints()
{
    return controlPoints;
}

void WidgetAnotation::mouseReleaseEvent(QMouseEvent *event)
{
    if (!imageLoaded)
    {
        return;
    }

    QPointF scenePoint = mapToScene(event->pos());
    double x = scenePoint.x();
    double y = scenePoint.y();

    if (event->button() == Qt::LeftButton)
        {
        if (draggingIndex == -1)
        {
            controlPoints.push_back(P(x, y));
            QGraphicsEllipseItem *circle = scene->addEllipse(x-2, y-2, 4, 4, yellowPen);
            circle->setBrush(QBrush(Qt::yellow));
            splineControlPointsCircles << circle;

            int count = splineControlPointsCircles.count();
            if (count >= 2)
            {
                splineControlPointsCircles[count - 2]->setPen(whitePen);
                splineControlPointsCircles[count - 2]->setBrush(QBrush(Qt::white));
            }

            clearLines();
            drawLines();
        }
        else
        {
            draggingIndex = -1;
        }
    }
    else if (event->button() == Qt::RightButton)
    {
        // get nearest point
        int nearest = getNearestPoint(x, y, 10);
        if (nearest != -1)
        {
            scene->removeItem(splineControlPointsCircles[nearest]);
            splineControlPointsCircles.remove(nearest);
            controlPoints.erase(controlPoints.begin()+nearest);

            int count = splineControlPointsCircles.count();
            if (count >= 1)
            {
                splineControlPointsCircles[count - 1]->setPen(yellowPen);
                splineControlPointsCircles[count - 1]->setBrush(QBrush(Qt::yellow));
            }
        }

        clearLines();
        drawLines();
    }
    else if (event->button() == Qt::MiddleButton)
    {
        draggingAll = false;
    }
}

void WidgetAnotation::clearLines()
{
    foreach(QGraphicsLineItem *line, splineLines)
    {
        QGraphicsItem *i = (QGraphicsItem*)line;
        scene->removeItem(i);
    }
    splineLines.clear();
}

void WidgetAnotation::clearControlCircles()
{
    foreach(QGraphicsEllipseItem *e, splineControlPointsCircles)
    {
        QGraphicsItem *i = (QGraphicsItem*)e;
        scene->removeItem(i);
    }
    splineControlPointsCircles.clear();
}

void WidgetAnotation::drawLines()
{
    Points splinePoints = spline.getSplinePoints(controlPoints);
    int n = splinePoints.size();
    for (int i = 1; i < n; i++)
    {
        P &prev = splinePoints[i-1];
        P &next = splinePoints[i];

        QGraphicsLineItem *line = scene->addLine(prev.x, prev.y, next.x, next.y, whitePen);
        splineLines << line;
    }
}

void WidgetAnotation::drawControlCircles()
{
    int n = controlPoints.size();
    for (int i = 0; i < n; i++)
    {
        splineControlPointsCircles << scene->addEllipse(controlPoints[i].x - 2, controlPoints[i].y - 2, 4, 4,
                                                        (i == n-1) ? yellowPen : whitePen,
                                                        QBrush((i == n-1) ? Qt::yellow : Qt::white));
    }
}

void WidgetAnotation::mousePressEvent(QMouseEvent *event)
{
    if (!imageLoaded)
    {
        emit noImage();
        return;
    }

    if (event->button() == Qt::LeftButton)
    {
        QPointF scenePoint = mapToScene(event->pos());
        double x = scenePoint.x();
        double y = scenePoint.y();

        // clicked near some point?
        mouseDownPos = event->pos();
        draggingIndex = getNearestPoint(x, y, 10);
    }
    else if (event->button() == Qt::MiddleButton)
    {
        draggingAll = true;
        mouseDownPos = event->pos();
    }
}

int WidgetAnotation::getNearestPoint(double x, double y, double threshold)
{
    int n = controlPoints.size();
    for (int i = 0; i < n; i++)
    {
        P p(x, y);
        double d = Common::eucl(p, controlPoints[i]);
        if (d < threshold)
        {
            return i;
        }
    }
    return -1;
}

void WidgetAnotation::mouseMoveEvent(QMouseEvent *event)
{
    if (draggingIndex >= 0)
    {
        QPointF scenePoint = mapToScene(event->pos());
        double x = scenePoint.x();
        double y = scenePoint.y();

        QGraphicsEllipseItem *c = splineControlPointsCircles[draggingIndex];
        c->setRect(x-2, y-2, 4, 4);

        controlPoints[draggingIndex] = P(x,y);

        clearLines();
        drawLines();
    }
    else if (draggingAll)
    {
        int dx = event->pos().x() - mouseDownPos.x();
        int dy = event->pos().y() - mouseDownPos.y();

        for (int i = 0; i < splineControlPointsCircles.size(); i++)
        {
            splineControlPointsCircles[i]->moveBy(dx, dy);
        }
        for (int i = 0; i < splineLines.size(); i++)
        {
            splineLines[i]->moveBy(dx, dy);
        }
        for (int i = 0; i < controlPoints.size(); i++)
        {
            controlPoints[i] = P(controlPoints[i].x+dx, controlPoints[i].y+dy);
        }

        mouseDownPos = event->pos();
    }

}
