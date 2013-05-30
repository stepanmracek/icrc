#ifndef DIALOGANOTATION_H
#define DIALOGANOTATION_H

#include <QDialog>

#include "linalg/common.h"

namespace Ui {
class DialogAnotation;
}

class DialogAnotation : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogAnotation(QWidget *parent = 0);
    ~DialogAnotation();

    Points getControlPoints();

    int getShapeWidth();

public slots:
    void setImage(QPixmap pixmap);
    
private slots:
    void on_spinBox_valueChanged(int newValue);

private:
    Ui::DialogAnotation *ui;
};

#endif // DIALOGANOTATION_H
