#ifndef WINDOWANOTATIONMANAGER_H
#define WINDOWANOTATIONMANAGER_H

#include <QMainWindow>
#include <QListWidgetItem>

#include "strain/shapetracker.h"

namespace Ui {
class WindowAnotationManager;
}

class WindowAnotationManager : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit WindowAnotationManager(QString path, ShapeTracker tracker, QWidget *parent = 0);
    ~WindowAnotationManager();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void setDirectory(QString path);
    void loadFile(QString fileName);

    void on_btnBrowse_clicked();

    void on_btnLoad_clicked();

    void on_listFiles_itemDoubleClicked(QListWidgetItem *item);

    void on_btnAnotate_clicked();

    void on_btnTrack_clicked();

    void on_btnCoordSystem_clicked();

    void on_btnMetadata_clicked();

    void on_btnStats_clicked();

    void on_widgetResult_noImage();

private:
    Ui::WindowAnotationManager *ui;
    ShapeTracker tracker;
    QString path;
};

#endif // WINDOWANOTATIONMANAGER_H
