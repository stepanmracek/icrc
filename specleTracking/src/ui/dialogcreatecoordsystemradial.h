#ifndef DIALOGCREATCOORDSYSTEMRADIAL_H
#define DIALOGCREATCOORDSYSTEMRADIAL_H

#include <QDialog>

#include "linalg/common.h"
#include "strain/coordsystem.h"

namespace Ui {
class DialogCreatCoordSystemRadial;
}

class DialogCreateCoordSystemRadial : public QDialog
{
    Q_OBJECT

    CoordSystemRadial *coordSystem;
    Mat8 originalImage;
    
public:
    explicit DialogCreateCoordSystemRadial(Mat8 &image, CoordSystemRadial *coordSystem, QWidget *parent = 0);
    ~DialogCreateCoordSystemRadial();

    CoordSystemBase *getNewCoordSystem();
    
public slots:
    void doubleValueChanged(double arg1);
    void intValueChanged(int arg1);

private slots:
    void on_buttonBox_accepted();

private:
    bool loadCompleted;
    Ui::DialogCreatCoordSystemRadial *ui;

    void recalculate();
    void drawResult(CoordSystemRadial *c);
    bool check();
};

#endif // DIALOGCREATCOORDSYSTEMRADIAL_H
