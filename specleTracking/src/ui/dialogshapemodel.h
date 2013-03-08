#ifndef DIALOGSHAPEMODEL_H
#define DIALOGSHAPEMODEL_H

#include <QDialog>
#include <QSlider>
#include <QList>

#include "linalg/backprojectionbase.h"
#include "strain/shapetracker.h"

namespace Ui {
class DialogShapeModel;
}

class DialogShapeModel : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogShapeModel(BackProjectionBase *backProjectionBase, ShapeTracker *tracker, QWidget *parent = 0);
    ~DialogShapeModel();
    
private:
    Ui::DialogShapeModel *ui;

    QList<QSlider*> sliders;
    BackProjectionBase *backProjectionBase;
    ShapeTracker *tracker;

private slots:
    void drawShapeModel();
};

#endif // DIALOGSHAPEMODEL_H
