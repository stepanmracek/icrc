#ifndef DIALOGCREATETRACKER_H
#define DIALOGCREATETRACKER_H

#include <QDialog>

#include "strain/shapetracker.h"

namespace Ui {
class DialogCreateTracker;
}

class DialogCreateTracker : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogCreateTracker(QWidget *parent = 0);
    ~DialogCreateTracker();

    ShapeTracker *getNewShapeTracker() { return newShapeTracker; }
    
private:
    ShapeTracker *newShapeTracker;

    Ui::DialogCreateTracker *ui;

    QString modelNone;
    QString modelPCANonIter;
    QString modelPCAIter;

    QString pointTrackerSSD;
    QString pointTrackerCorr;
    QString pointTrackerOptical;

    QString weights1;
    QString weights2;
    QString weights3;
    QString weights4;

private slots:
    void onCreateNewTrackerRequest();
};

#endif // DIALOGCREATETRACKER_H
