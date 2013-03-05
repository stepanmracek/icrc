#ifndef DIALOGIMAGEPROCESSING_H
#define DIALOGIMAGEPROCESSING_H

#include <QDialog>
#include <QAbstractButton>

#include "strain/imagefilter.h"
#include "linalg/common.h"
#include "strain/coordsystem.h"

namespace Ui {
class DialogImageProcessing;
}

class DialogImageProcessing : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogImageProcessing(const Mat8 &frame, CoordSystemBase *coordSystem, QWidget *parent = 0);
    ~DialogImageProcessing();

    QList<ImageFilterBase*> getFilters();
    
private slots:
    void on_btnAdd_clicked();

    void on_btnRemove_clicked();

    void on_btnApply_clicked();

private:
    Ui::DialogImageProcessing *ui;
    QStringList filterNames;
    Mat8 image;

    ImageFilterBase *createImageFilter(const QString &name);
};

#endif // DIALOGIMAGEPROCESSING_H
