#include "widgetrainbow.h"

#include <QPainter>
#include <QDebug>
#include <QLinearGradient>

WidgetRainbow::WidgetRainbow(QWidget *parent) :
    QWidget(parent)
{
}

void WidgetRainbow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    int h = height();
    for (int i = 0; i < h; i++)
    {
        double hue = 1.0 - ((double)i)/h;
        QColor color = QColor::fromHsvF(hue, 1, 1);
        painter.setPen(QPen(color));
        painter.drawLine(0, i, width(), i);
    }
}
