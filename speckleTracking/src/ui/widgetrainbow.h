#ifndef WIDGETRAINBOW_H
#define WIDGETRAINBOW_H

#include <QWidget>

class WidgetRainbow : public QWidget
{
    Q_OBJECT
public:
    explicit WidgetRainbow(QWidget *parent = 0);

protected:
    void paintEvent(QPaintEvent *);
    
signals:
    
public slots:
    
};

#endif // WIDGETRAINBOW_H
