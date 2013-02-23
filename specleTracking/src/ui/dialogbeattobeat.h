#ifndef DIALOGBEATTOBEAT_H
#define DIALOGBEATTOBEAT_H

#include <QDialog>

#include "strain/strainstatistics.h"

namespace Ui {
class DialogBeatToBeat;
}

class DialogBeatToBeat : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogBeatToBeat(QVector<StrainStatistics> &beatsStats, QWidget *parent = 0);
    ~DialogBeatToBeat();
    
private:
    Ui::DialogBeatToBeat *ui;
};

#endif // DIALOGBEATTOBEAT_H
