#ifndef DIALOGBEATTOBEAT_H
#define DIALOGBEATTOBEAT_H

#include <QDialog>

#include "strain/strainstatistics.h"
#include "strain/strain.h"

namespace Ui {
class DialogBeatToBeat;
}

class DialogBeatToBeat : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogBeatToBeat(Strain *strainModel, QVector<StrainStatistics> &beatsStats, QWidget *parent = 0);
    ~DialogBeatToBeat();

private slots:
    void on_comboBox_activated(const QString &text);

private:
    Ui::DialogBeatToBeat *ui;
    Strain *strainModel;
    QVector<StrainStatistics> &beatsStats;

    void addMainStrain();
    void addAllSegments();
    void addSegment(int index);
};

#endif // DIALOGBEATTOBEAT_H
