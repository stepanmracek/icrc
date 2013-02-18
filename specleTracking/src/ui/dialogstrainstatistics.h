#ifndef DIALOGSTRAINSTATISTICS_H
#define DIALOGSTRAINSTATISTICS_H

#include <QDialog>
#include <QMap>

#include "strain/strainstatistics.h"
#include "strain/videodatabase.h"
#include "strain/videodataclip.h"
#include "strain/shapetracker.h"

namespace Ui {
class DialogStrainStatistics;
}

class DialogStrainStatistics : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogStrainStatistics(QWidget *parent = 0);
    ~DialogStrainStatistics();

    void SetData(StrainStatistics *strainStatistics, ShapeTracker *tracker, VideoDataClip &clip, QMap<int, Points> &shapes)
    
private:
    Ui::DialogStrainStatistics *ui;
};

#endif // DIALOGSTRAINSTATISTICS_H
