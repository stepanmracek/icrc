#ifndef WIDGETANOTATIONMANAGER_H
#define WIDGETANOTATIONMANAGER_H

#include <QWidget>
#include <QListWidgetItem>
#include <QMap>

#include "strain/shapetracker.h"
#include "strain/videodataclip.h"
#include "linalg/common.h"

namespace Ui {
class WidgetAnotationManager;
}

class WidgetAnotationManager : public QWidget
{
    Q_OBJECT
    
public:
    explicit WidgetAnotationManager(QString path, ShapeTracker *tracker, QWidget *parent = 0);
    ~WidgetAnotationManager();

    ShapeTracker *tracker;
    
private slots:
    void setDirectory(QString path);
    void loadFile(QString fileName);
    //void deserializeShapes();

    void on_btnBrowse_clicked();

    void on_btnLoad_clicked();

    //void on_btnPrev_clicked();

    //void on_btnNext_clicked();

    //void on_horizontalSlider_valueChanged(int value);

    void on_listFiles_itemDoubleClicked(QListWidgetItem *);

    void on_btnTrack_clicked();

    void on_widgetResult_noImage();

    void on_btnStats_clicked();

    void on_btnAnotate_clicked();

    void on_btnCoordSystem_clicked();

    void on_btnMetadata_clicked();

protected:
    void closeEvent(QCloseEvent *event);

private:
    Ui::WidgetAnotationManager *ui;
    QString path;
    //QString currentFilename;
    //VideoDataClip clip;
    //QMap<int, Points > shapes;
    //int currentIndex;

};

#endif // WIDGETANOTATIONMANAGER_H
