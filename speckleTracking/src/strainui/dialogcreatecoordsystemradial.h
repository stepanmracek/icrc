#ifndef DIALOGCREATCOORDSYSTEMRADIAL_H
#define DIALOGCREATCOORDSYSTEMRADIAL_H

#include <QDialog>
#include <QEvent>
#include <QPoint>

#include "linalg/common.h"
#include "strain/coordsystem.h"
#include "dialogcreatecoordsystem.h"

namespace Ui {
class DialogCreatCoordSystemRadial;
}

class DialogCreateCoordSystemRadial : public DialogCreateCoordSystem
{
    Q_OBJECT

    CoordSystemRadial *coordSystem;
    Mat8 originalImage;
    
public:
    explicit DialogCreateCoordSystemRadial(Mat8 &image, CoordSystemRadial *coordSystem, QWidget *parent = 0);
    ~DialogCreateCoordSystemRadial();

    CoordSystemRadial *getNewCoordSystem();
    
public slots:
    void doubleValueChanged(double arg1);
    void intValueChanged(int arg1);

private slots:
    void on_buttonBox_accepted();

    void on_gvOriginal_leftMouseClick(const QPoint &point);

private:
    Ui::DialogCreatCoordSystemRadial *ui;
    bool loadCompleted;

    void recalculate();
    void drawResult(CoordSystemRadial *c);
    bool check();
};

#endif // DIALOGCREATCOORDSYSTEMRADIAL_H
