#include "windowanotationmanager.h"
#include "ui_windowanotationmanager.h"

#include <QFileDialog>
#include <QFile>
#include <QDebug>
#include <QInputDialog>
#include <QMainWindow>
#include <QMessageBox>
#include <QProgressDialog>
#include <QKeyEvent>
#include <algorithm>

#include "ui/uiutils.h"
#include "ui/dialogstrainstatistics.h"
#include "linalg/serialization.h"
#include "strain/shapeprocessing.h"
#include "strain/pointtrackerdistance.h"
#include "strain/pointtrackeropticalflow.h"
#include "strain/pointtrackerneighbouropticalflow.h"
#include "ui/dialoganotation.h"
#include "ui/dialogcreatecoordsystemradial.h"
#include "ui/dialogvideodataclipmetadata.h"
#include "ui/dialogbeattobeat.h"
#include "ui/dialogimageprocessing.h"
#include "ui/dialogshapemodel.h"
#include "strain/strainresultprocessing.h"

WindowAnotationManager::WindowAnotationManager(const QString &path, const QString &dataDir, ShapeTracker *tracker, QWidget *parent) :
    dataDir(dataDir),
    QMainWindow(parent),
    ui(new Ui::WindowAnotationManager)
{
    ui->setupUi(this);
    setTracker(tracker);
    setDirectory(path);
}

WindowAnotationManager::~WindowAnotationManager()
{
    delete ui;
}

void WindowAnotationManager::setTracker(ShapeTracker *tracker)
{
    this->tracker = tracker;
    ui->widgetStrainVideo->setTracker(tracker);
    tracker->setParent(this);
    updateTrackerInfo();
}

void WindowAnotationManager::updateTrackerInfo()
{
    QString text = tracker->getInfo();
    ui->textEditTrackerInfo->setText(text);
}

void WindowAnotationManager::saveMetadataAndShapes()
{
    VideoDataClip *clip = ui->widgetStrainVideo->getClip();
    if (!clip || clip->size() == 0) return;

    if (QMessageBox::question(this, "Question", "Save metadata?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
        ui->widgetStrainVideo->serializeMetadata(path);

    if (ui->widgetStrainVideo->shapes.size() == 0) return;
    if (QMessageBox::question(this, "Question", "Save shapes?", QMessageBox::Yes, QMessageBox::No) == QMessageBox::Yes)
        ui->widgetStrainVideo->serializeShapes(path);

}

void WindowAnotationManager::closeEvent(QCloseEvent *event)
{
    saveMetadataAndShapes();
}

void WindowAnotationManager::on_btnBrowse_clicked()
{
    QString selected = QFileDialog::getExistingDirectory(this, QString(), path);
    if (selected.isNull() || selected.isEmpty()) return;

    saveMetadataAndShapes();
    setDirectory(selected);
}

void WindowAnotationManager::setDirectory(QString path)
{
    this->path = path;
    ui->lineEdit->setText(path);
    ui->listFiles->clear();

    QDir dir(path);
    if (!dir.exists()) return;

    ui->widgetStrainVideo->unload();
    QStringList filter; filter << "*.wmv" << "*.WMV" << "*.avi" << "*.AVI";
    QStringList files = dir.entryList(filter, QDir::Files | QDir::NoDotAndDotDot, QDir::Name);

    ui->listFiles->addItems(files);

    if (files.count() > 0)
    {
        ui->listFiles->setCurrentRow(0);
        on_btnLoad_clicked();
    }
}

void WindowAnotationManager::on_btnLoad_clicked()
{
    QList<QListWidgetItem*> items = ui->listFiles->selectedItems();
    if (items.count() == 0) return;

    loadFile(items[0]->text());
}

void WindowAnotationManager::on_listFiles_itemDoubleClicked(QListWidgetItem *item)
{
    on_btnLoad_clicked();
}

void WindowAnotationManager::loadFile(QString fileName)
{
    QString fullPath = path + QDir::separator() + fileName;
    QFileInfo f(fullPath);
    if (!f.exists()) return;

    qDebug() << "Loading" << fileName;
    saveMetadataAndShapes();
    ui->widgetStrainVideo->load(path, fileName);
    ui->listViewBeats->setModel(new ModelListOfInts(ui->widgetStrainVideo->getClip()->getMetadata()->beatIndicies, this));
}

void WindowAnotationManager::on_btnAnotate_clicked()
{
    VideoDataClip *clip = ui->widgetStrainVideo->getClip();
    if (!clip || clip->size() == 0) return;

    const Mat8 &frame = clip->frames[ui->widgetStrainVideo->getCurrentIndex()];
    QPixmap image = UIUtils::Mat8ToQPixmap(frame);
    DialogAnotation dlgAnotation(this);
    dlgAnotation.setImage(image);
    int dlgResult = dlgAnotation.exec();

    if (dlgResult == QDialog::Accepted)
    {
        ui->widgetStrainVideo->setControlPoints(dlgAnotation.getControlPoints(), dlgAnotation.getShapeWidth());
    }
}

void WindowAnotationManager::on_btnTrack_clicked()
{
    VideoDataClip *clip = ui->widgetStrainVideo->getClip();
    if (!clip || clip->size() == 0) return;

    // check if clip has beat indicies
    if (clip->getMetadata()->beatIndicies.count() == 0)
    {
        QMessageBox msg(QMessageBox::Information, "Information", "Missing beat indicies.");
        msg.exec();
        return;
    }

    int beatStart = 0;
    int beatEnd = clip->size();
    int currentIndex = ui->widgetStrainVideo->getCurrentIndex();
    clip->getBeatRange(currentIndex, beatStart, beatEnd);
    qDebug() << "Beat range:" << beatStart << beatEnd;

    // check if the beat start is anotated
    if (!ui->widgetStrainVideo->shapes.contains(beatStart))
    {
        QMessageBox msg(QMessageBox::Information, "Information", "Beat start is not anotated");
        msg.exec();
        return;
    }


    if (beatEnd <= beatStart)
    {
        qDebug() << "Wrong beatRange";
        return;
    }


    int frames = beatEnd - beatStart - 1;
    QProgressDialog progress("Processing...", "Cancel", 0, frames);
    progress.setModal(Qt::WindowModal);

    ShapeMap newShapes = tracker->track(ui->widgetStrainVideo->getClip(),
                                        beatStart, beatEnd,
                                        ui->widgetStrainVideo->shapes[beatStart],
                                        &progress);

    QMapIterator<int, Points> iter(newShapes);
    while (iter.hasNext())
    {
        iter.next();
        ui->widgetStrainVideo->shapes[iter.key()] = iter.value();
    }

    /*for (int i = 0; i < frames; i++)
    {
        progress.setValue(i);
        //qDebug() << "Tracking" << i << "/" << frames;

        if (progress.wasCanceled()) break;

        // prepare previous shapes and previous frames
        VectorOfShapes prevShapes;
        VectorOfImages prevFrames;

        for (unsigned int j = 0; j < tracker->weights.size(); j++)
        {
            int prevIndex = beatStart+i-j;
            //qDebug() << "  j:" << j << "prevIndex:" << prevIndex;
            if (prevIndex < beatStart)
            {
                //qDebug() << "  prevIndex < currentIndex";
                break;
            }
            if (prevIndex == limit)
            {
                //qDebug() << "  prevIndex == limit";
                break;
            }

            prevShapes.push_back(ui->widgetStrainVideo->shapes[prevIndex]);
            prevFrames.push_back(clip->frames[prevIndex]);
        }
        //qDebug() << "  prevShape/prevFrames size:" << prevShapes.size() << prevFrames.size();
        if (prevShapes.size() == 0) break;

        int nextIndex = beatStart+i+1;
        //qDebug() << "  nextIndex:" << nextIndex;
        if (nextIndex == limit) break;
        Mat8 nextFrame = clip->frames[nextIndex];

        //qDebug() << "  reversing prevShapes and prevFrames";
        std::reverse(prevShapes.begin(), prevShapes.end());
        std::reverse(prevFrames.begin(), prevFrames.end());
        //qDebug() << "  tracking";
        Points nextShape = tracker->track(prevFrames, prevShapes, nextFrame, clip->getMetadata()->getCoordSystem());
        ui->widgetStrainVideo->shapes[nextIndex] = nextShape;
    }

    progress.setValue(frames);*/
}

void WindowAnotationManager::on_btnCoordSystem_clicked()
{
    VideoDataClip *clip = ui->widgetStrainVideo->getClip();
    if (!clip || clip->size() == 0) return;

    Mat8 frame = clip->frames[ui->widgetStrainVideo->getCurrentIndex()];
    DialogCreateCoordSystemRadial dlg(frame, clip->getMetadata()->getCoordSystem() , this);
    if (dlg.exec() == QDialog::Accepted)
    {
        clip->getMetadata()->getCoordSystem()->init(dlg.getNewCoordSystem());
        ui->widgetStrainVideo->display(ui->widgetStrainVideo->getCurrentIndex());
    }
}

/*void WindowAnotationManager::on_btnMetadata_clicked()
{
    VideoDataClip *clip = ui->widgetStrainVideo->getClip();
    if (!clip || clip->size() == 0) return;

    DialogVideoDataClipMetadata dlg(clip->getMetadata());
    if (dlg.exec() == QDialog::Accepted)
    {
        clip->getMetadata()->beatIndicies = dlg.getMetadata()->beatIndicies;
    }
}*/

void WindowAnotationManager::on_btnStats_clicked()
{
    VideoDataClip *clip = ui->widgetStrainVideo->getClip();
    if (!clip || clip->size() == 0) return;

    QMap<int, Points> &shapes = ui->widgetStrainVideo->shapes;
    int currentIndex = ui->widgetStrainVideo->getCurrentIndex();

    VectorOfShapes subShapes;
    QMap<int, Points> subShapesMap;
    VideoDataClip *subClip = new VideoDataClip();
    char *error = clip->getSubClip(currentIndex, shapes, subClip, subShapes, subShapesMap);
    if (error != 0)
    {
        QMessageBox msg(QMessageBox::Information, "Information", error);
        msg.exec();
        return;
    }

    if (subClip->size() == 0)
    {
        QMessageBox msg(QMessageBox::Information, "Information", "zero beat range");
        msg.exec();
        return;
    }

    StrainStatistics stats(tracker->getStrain(), subShapes);
    DialogStrainStatistics dlgStats;
    dlgStats.SetData(&stats, tracker, subClip, subShapesMap);
    dlgStats.exec();
}

void WindowAnotationManager::on_widgetResult_noImage()
{
    QMessageBox msg(QMessageBox::Information, "Information", "Load the image first");
    msg.exec();
}

void WindowAnotationManager::on_btnBeatToBeat_clicked()
{
    VideoDataClip *clip = ui->widgetStrainVideo->getClip();
    if (!clip || clip->size() == 0) return;

    QVector<StrainStatistics> allBeatsStats = StrainStatistics::getAllBeatsStats(clip, tracker->getStrain(),
                                                                                 ui->widgetStrainVideo->shapes);

    DialogBeatToBeat dlg(tracker->getStrain(), allBeatsStats);
    dlg.exec();
}

void WindowAnotationManager::on_actionChangeImageProcessing_triggered()
{
    VideoDataClip *clip = ui->widgetStrainVideo->getClip();
    if (!clip || clip->size() == 0) return;

    DialogImageProcessing dlg(clip->frames[0], clip->getMetadata()->getCoordSystem());
    if (dlg.exec() == QDialog::Accepted)
    {
        tracker->addFilters(dlg.getFilters());
        updateTrackerInfo();
    }
}

void WindowAnotationManager::on_actionShowShapeModel_triggered()
{
    ShapeNormalizerBase *normalizer = tracker->getStrain()->getShapeNormalizer();
    ShapeNormalizerShapeModel *n = qobject_cast<ShapeNormalizerShapeModel*>(normalizer);
    if (n != 0)
    {
        DialogShapeModel dlg(n->getShapeModel()->getBackProjection(), tracker);
        dlg.exec();
    }
    else
    {
        QMessageBox msg(QMessageBox::Information, "Information",
                        "Current tracker is not based on statistical shape model");
        msg.exec();
    }
}

void WindowAnotationManager::on_actionSAD_triggered()
{
    int windowSize = QInputDialog::getInteger(this, "Window size", "Window size:", 21, 1, 100, 2);
    PointTrackerBase *pTracker = new PointTrackerDistance(new CityblockMetric(), windowSize);
    tracker->setPointTracker(pTracker);
    updateTrackerInfo();
}

void WindowAnotationManager::on_actionSSD_triggered()
{
    int windowSize = QInputDialog::getInteger(this, "Window size", "Window size:", 21, 1, 100, 2);
    PointTrackerBase *pTracker = new PointTrackerDistance(new SumOfSquareDifferences(), windowSize);
    tracker->setPointTracker(pTracker);
    updateTrackerInfo();
}

void WindowAnotationManager::on_actionOptical_flow_triggered()
{
    int (outlier) = QInputDialog::getInteger(this, "Outlier distance", "Outlier distance:", 20, 5, 100, 1);
    PointTrackerBase *pTracker = new PointTrackerOpticalFlow(outlier);
    tracker->setPointTracker(pTracker);
    updateTrackerInfo();
}

void WindowAnotationManager::on_actionOptical_flow_with_neighbourhood_triggered()
{
    int (outlier) = QInputDialog::getInteger(this, "Outlier distance", "Outlier distance:", 20, 5, 100, 1);
    int windowSize = QInputDialog::getInteger(this, "Window size", "Window size:", 21, 1, 100, 2);
    int step = QInputDialog::getInteger(this, "Step within window", "Step within window:", 5, 1, 10, 1);
    PointTrackerBase *pTracker = new PointTrackerNeighbourOpticalFlow(outlier, windowSize, step);
    tracker->setPointTracker(pTracker);
    updateTrackerInfo();
}

void WindowAnotationManager::on_actionCorrelation_triggered()
{
    int windowSize = QInputDialog::getInteger(this, "Window size", "Window size:", 21, 1, 100, 2);
    PointTrackerBase *pTracker = new PointTrackerDistance(new CorrelationMetric(), windowSize);
    tracker->setPointTracker(pTracker);
    updateTrackerInfo();
}

void WindowAnotationManager::on_actionCosine_triggered()
{
    int windowSize = QInputDialog::getInteger(this, "Window size", "Window size:", 21, 1, 100, 2);
    PointTrackerBase *pTracker = new PointTrackerDistance(new CosineMetric(), windowSize);
    tracker->setPointTracker(pTracker);
    updateTrackerInfo();
}

void WindowAnotationManager::on_actionQuit_triggered()
{
    this->close();
}

void WindowAnotationManager::on_actionNone_triggered()
{
    ShapeNormalizerBase* normalizer = new ShapeNormalizerPass();
    tracker->getStrain()->SetShapeNormalizer(normalizer);
    updateTrackerInfo();
}

void WindowAnotationManager::on_actionPCA_triggered()
{
    PCA *pca = new PCA(dataDir + QDir::separator() + "pca-shape");
    StatisticalShapeModel *model = new StatisticalShapeModel(pca);
    ShapeNormalizerBase* normalizer = new ShapeNormalizerIterativeStatisticalShape(model);
    tracker->getStrain()->SetShapeNormalizer(normalizer);
    updateTrackerInfo();
}

void WindowAnotationManager::on_btnAddBeat_clicked()
{
    VideoDataClip *clip = ui->widgetStrainVideo->getClip();
    if (!clip || clip->size() == 0) return;

    int beat = ui->widgetStrainVideo->getCurrentIndex();

    QAbstractItemModel *model = ui->listViewBeats->model();
    int rowCount = model->rowCount();
    model->insertRow(rowCount);
    QModelIndex index = model->index(rowCount, 0);
    model->setData(index, beat);
}

void WindowAnotationManager::on_btnRemoveBeat_clicked()
{
    VideoDataClip *clip = ui->widgetStrainVideo->getClip();
    if (!clip || clip->size() == 0) return;

    QAbstractItemModel *model = ui->listViewBeats->model();
    QModelIndex modelIndex = ui->listViewBeats->currentIndex();
    if (modelIndex.row() < 0) return;

    model->removeRow(modelIndex.row());
}

void WindowAnotationManager::on_actionWeights1_triggered()
{
    VectorF weights;
    weights.push_back(1.0);
    tracker->weights = weights;
    updateTrackerInfo();
}

void WindowAnotationManager::on_actionWeights2_triggered()
{
    VectorF weights;
    weights.push_back(1.0); weights.push_back(0.5);
    tracker->weights = weights;
    updateTrackerInfo();
}

void WindowAnotationManager::on_actionWeights3_triggered()
{
    VectorF weights;
    weights.push_back(1.0); weights.push_back(0.5); weights.push_back(0.25);
    tracker->weights = weights;
    updateTrackerInfo();
}

void WindowAnotationManager::on_actionWeights4_triggered()
{
    VectorF weights;
    weights.push_back(1.0); weights.push_back(0.5); weights.push_back(0.25);  weights.push_back(0.125);
    tracker->weights = weights;
    updateTrackerInfo();
}


void WindowAnotationManager::on_actionResProcAvg_triggered()
{
    int windowSize = QInputDialog::getInteger(this, "Window size", "Window size:", 3, 1, 51, 2);
    StrainResProcFloatingAvg *proc = new StrainResProcFloatingAvg(windowSize);
    tracker->setResultProcessing(proc);
    updateTrackerInfo();
}

void WindowAnotationManager::on_actionResProcNone_triggered()
{
    StrainResultProcessingPass *proc = new StrainResultProcessingPass();
    tracker->setResultProcessing(proc);
    updateTrackerInfo();
}
