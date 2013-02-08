#ifndef WIDGETRESULT_H
#define WIDGETRESULT_H

#include <QWidget>
#include <QGraphicsView>

#include "linalg/common.h"
#include "strain/strain.h"
#include "strain/shapetracker.h"
#include "strain/videodataclip.h"

class WidgetResult : public QGraphicsView
{
    Q_OBJECT

private:
    ShapeTracker *tracker;
    QList<QGraphicsItem*> graphicItems;
    QList<QGraphicsItem*> coordGraphicItems;
    Points resultPoints;
    bool imageSet;

public:
    explicit WidgetResult(QWidget *parent = 0);

    void setTracker(ShapeTracker *tracker) { this->tracker = tracker; }

    Points getResultPoints() { return resultPoints; }

protected:
    QGraphicsScene *scene;
    
signals:
    void trackerNotSet();
    void imageNotSet();
    
public slots:

    void setImage(QPixmap pixmap);
    void setControlPoints(Points controlPoints, int shapeWidth);
    void setResultPoints(Points points);

};

#endif // WIDGETRESULT_H
