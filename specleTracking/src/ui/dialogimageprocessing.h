#ifndef DIALOGIMAGEPROCESSING_H
#define DIALOGIMAGEPROCESSING_H

#include <QDialog>

namespace Ui {
class DialogImageProcessing;
}

class DialogImageProcessing : public QDialog
{
    Q_OBJECT
    
public:
    explicit DialogImageProcessing(QWidget *parent = 0);
    ~DialogImageProcessing();
    
private:
    Ui::DialogImageProcessing *ui;
};

#endif // DIALOGIMAGEPROCESSING_H
