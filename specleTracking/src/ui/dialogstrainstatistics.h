#ifndef DIALOGSTRAINSTATISTICS_H
#define DIALOGSTRAINSTATISTICS_H

#include <QDialog>

namespace Ui {
class DialogStrainStatistics;
}

class DialogStrainStatistics : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogStrainStatistics(QWidget *parent = 0);
    ~DialogStrainStatistics();
    
private:
    Ui::DialogStrainStatistics *ui;
};

#endif // DIALOGSTRAINSTATISTICS_H
