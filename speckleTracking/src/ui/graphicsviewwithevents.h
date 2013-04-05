#ifndef GRAPHICSVIEWWITHEVENTS_H
#define GRAPHICSVIEWWITHEVENTS_H

#include <QGraphicsView>
#include <QPoint>

class GraphicsViewWithEvents : public QGraphicsView
{
    Q_OBJECT
public:
    explicit GraphicsViewWithEvents(QWidget *parent = 0);
    
protected:
    void mouseReleaseEvent(QMouseEvent *event);

signals:
    void leftMouseClick(QPoint point);
};

#endif // GRAPHICSVIEWWITHEVENTS_H
