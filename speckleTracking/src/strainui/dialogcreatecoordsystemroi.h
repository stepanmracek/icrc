#ifndef DIALOGCREATECOORSYSTEMROI_H
#define DIALOGCREATECOORSYSTEMROI_H

#include <QDialog>

#include "dialogcreatecoordsystem.h"
#include "strain/coordsystem.h"

namespace Ui {
class DialogCreateCoordSystemROI;
}

class DialogCreateCoordSystemROI : public DialogCreateCoordSystem
{
    Q_OBJECT

    CoordSystemROI *coordSystem;
    Mat8 originalImage;

public:
    explicit DialogCreateCoordSystemROI(Mat8 &image, CoordSystemROI *coordSystem, QWidget *parent = 0);
    ~DialogCreateCoordSystemROI();

    CoordSystemROI *getNewCoordSystem();

private slots:
    void intValueChanged(int val);

private:
    Ui::DialogCreateCoordSystemROI *ui;
    bool loadCompleted;

    void recalculate();
    void drawResult(CoordSystemROI *c);
};

#endif // DIALOGCREATECOORSYSTEMROI_H
