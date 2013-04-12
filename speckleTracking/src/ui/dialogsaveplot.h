#ifndef DIALOGSAVEPLOT_H
#define DIALOGSAVEPLOT_H

#include <QDialog>

namespace Ui {
class DialogSavePlot;
}

class DialogSavePlot : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogSavePlot(QWidget *parent = 0);
    ~DialogSavePlot();
    
private:
    Ui::DialogSavePlot *ui;
};

#endif // DIALOGSAVEPLOT_H
