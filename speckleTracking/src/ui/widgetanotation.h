#ifndef WIDGETANOTATION_H
#define WIDGETANOTATION_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsEllipseItem>

#include "linalg/spline.h"

class WidgetAnotation : public QGraphicsView
{
    Q_OBJECT
public:
    explicit WidgetAnotation(QWidget *parent = 0);

    void setShapeWidth(int width);
    void setImage(QPixmap pixmap);
    void setControlPoints(Points points);
    Points getControlPoints();

protected:
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    
    QGraphicsScene *scene;

private:
    QPen redPen;
    QPen whitePen;
    QPen yellowPen;
    int shapeWidth;

    Spline spline;
    Points controlPoints;
    QVector<QGraphicsLineItem*> splineLines;
    QVector<QGraphicsEllipseItem*> splineControlPointsCircles;

    bool draggingAll;
    bool imageLoaded;
    int draggingIndex;
    QPoint mouseDownPos;
    QPoint mouseUpPos;

    void clearLines();
    void clearControlCircles();
    void drawLines();
    void drawControlCircles();
    int getNearestPoint(double x, double y, double threshold);

signals:
    void noImage();
    
public slots:
    
};

#endif // WIDGETANOTATION_H
