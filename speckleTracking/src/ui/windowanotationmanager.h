#ifndef WINDOWANOTATIONMANAGER_H
#define WINDOWANOTATIONMANAGER_H

#include <QMainWindow>
#include <QListWidgetItem>

#include "strain/shapetracker.h"
#include "ui/modellistofints.h"

namespace Ui {
class WindowAnotationManager;
}

class WindowAnotationManager : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit WindowAnotationManager(const QString &path, const QString &dataDir, ShapeTracker *tracker, QWidget *parent = 0);
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

    void on_btnStats_clicked();

    void on_widgetResult_noImage();

    void on_btnBeatToBeat_clicked();

    void on_actionChangeImageProcessing_triggered();

    void on_actionShowShapeModel_triggered();

    void on_actionSAD_triggered();

    void on_actionSSD_triggered();

    void on_actionOptical_flow_triggered();

    void on_actionOptical_flow_with_neighbourhood_triggered();

    void on_actionCorrelation_triggered();

    void on_actionCosine_triggered();

    void on_actionQuit_triggered();

    void on_actionNone_triggered();

    void on_actionPCA_triggered();

    void on_btnAddBeat_clicked();

    void on_btnRemoveBeat_clicked();

    void on_actionWeights1_triggered();

    void on_actionWeights2_triggered();

    void on_actionWeights3_triggered();

    void on_actionWeights4_triggered();

    void on_actionResult_Processing_triggered();

    void on_actionResProcAvg_triggered();

    void on_actionResProcNone_triggered();

private:
    void setTracker(ShapeTracker *tracker);

    void updateTrackerInfo();

    void saveMetadataAndShapes();

    Ui::WindowAnotationManager *ui;
    ShapeTracker *tracker;
    QString path;
    QString dataDir;
};

#endif // WINDOWANOTATIONMANAGER_H
