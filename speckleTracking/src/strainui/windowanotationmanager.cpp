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

#include "uiutils.h"
#include "dialogstrainstatistics.h"
#include "linalg/serialization.h"
#include "strain/shapeprocessing.h"
#include "strain/pointtrackerdistance.h"
#include "strain/pointtrackeropticalflow.h"
#include "strain/pointtrackerneighbouropticalflow.h"
#include "dialoganotation.h"
#include "dialogcreatecoordsystemradial.h"
#include "dialogcreatecoordsystemroi.h"
#include "dialogvideodataclipmetadata.h"
#include "dialogbeattobeat.h"
#include "dialogimageprocessing.h"
#include "dialogshapemodel.h"
#include "strain/strainresultprocessing.h"
#include "strain/beatdetector.h"
#include "dialogbeatdetector.h"

WindowAnotationManager::WindowAnotationManager(const QString &path, ShapeTracker *tracker, QWidget *parent) :
    QMainWindow(parent), ui(new Ui::WindowAnotationManager)
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

void WindowAnotationManager::closeEvent(QCloseEvent *)
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
    QStringList filter; filter << "*.wmv" << "*.WMV" << "*.avi" << "*.AVI" << "*.mp4" << "*.MP4";
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

void WindowAnotationManager::on_listFiles_itemDoubleClicked(QListWidgetItem *)
{
    on_btnLoad_clicked();
}

void WindowAnotationManager::loadFile(QString fileName)
{
    QString fullPath = path + QDir::separator() + fileName;
    QFileInfo f(fullPath);
    if (!f.exists()) return;

    saveMetadataAndShapes();

    qDebug() << "Loading" << fileName;
    ui->widgetStrainVideo->load(path, fileName);
    updateBeatModel();
}

void WindowAnotationManager::updateBeatModel()
{
    ui->listViewBeats->selectionModel()->disconnect();
    ui->listViewBeats->setModel(new ModelListOfInts(ui->widgetStrainVideo->getClip()->getMetadata()->beatIndicies, this));
    connect(ui->listViewBeats->selectionModel(), SIGNAL(selectionChanged(QItemSelection,QItemSelection)), SLOT(onBeatSelectionChanged(QItemSelection, QItemSelection)));
}

void WindowAnotationManager::on_btnAnotate_clicked()
{
    VideoDataClip *clip = ui->widgetStrainVideo->getClip();
    if (!clip || clip->size() == 0) return;

    int curIndex = ui->widgetStrainVideo->getCurrentIndex();
    const Mat8 &frame = clip->frames[curIndex];
    QPixmap image = UIUtils::Mat8ToQPixmap(frame);
    DialogAnotation dlgAnotation(this);
    dlgAnotation.setImage(image);

    if (clip->getMetadata()->rawShapes.contains(curIndex))
    {
        dlgAnotation.setControlPoints(clip->getMetadata()->rawShapes[curIndex]);
    }

    int dlgResult = dlgAnotation.exec();
    if (dlgResult == QDialog::Accepted)
    {
        Points rawPoints = dlgAnotation.getControlPoints();
        if (rawPoints.size() > 1)
        {
            clip->getMetadata()->rawShapes[curIndex] = rawPoints;
            ui->widgetStrainVideo->setControlPoints(rawPoints, dlgAnotation.getShapeWidth());
        }
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

    int beatStart, beatEnd;
    int currentIndex = ui->widgetStrainVideo->getCurrentIndex();
    clip->getBeatRange(currentIndex, beatStart, beatEnd);
    qDebug() << "Beat range:" << beatStart << beatEnd;

    if (beatStart == -1)
    {
        QMessageBox msg(QMessageBox::Information, "Information", "Beat start is not defined");
        msg.exec();
        return;
    }
    if (beatEnd == -1)
    {
        QMessageBox msg(QMessageBox::Information, "Information", "Beat end is not defined");
        msg.exec();
        return;
    }
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
                                        ui->widgetStrainVideo->shapes.value(beatStart),
                                        &progress);

    qDebug() << newShapes.keys();

    QMapIterator<int, Points> iter(newShapes);
    while (iter.hasNext())
    {
        iter.next();
        ui->widgetStrainVideo->shapes[iter.key()] = iter.value();
    }
}

void WindowAnotationManager::on_btnCoordSystem_clicked()
{
    VideoDataClip *clip = ui->widgetStrainVideo->getClip();
    if (!clip || clip->size() == 0) return;

    Mat8 frame = clip->frames[ui->widgetStrainVideo->getCurrentIndex()];
    CoordSystemBase::Types type = clip->getMetadata()->getCoordSystem()->type();
    DialogCreateCoordSystem *dlg = 0;
    if (type == CoordSystemBase::TypeRadial)
    {
        dlg = new DialogCreateCoordSystemRadial(frame, (CoordSystemRadial*)clip->getMetadata()->getCoordSystem(), this);
    }
    if (type == CoordSystemBase::TypeROI)
    {
        dlg = new DialogCreateCoordSystemROI(frame, (CoordSystemROI*)clip->getMetadata()->getCoordSystem(), this);
    }
    if (dlg && dlg->exec() == QDialog::Accepted)
    {
        clip->getMetadata()->setCoordSystem(dlg->getNewCoordSystem());
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
    QString error = clip->getSubClip(currentIndex, shapes, subClip, subShapes, subShapesMap);
    if (!error.isEmpty())
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

    DialogBeatToBeat dlg(allBeatsStats);
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
    int windowSize = QInputDialog::getInt(this, "Window size", "Window size:", 21, 1, 100, 2);
    PointTrackerBase *pTracker = new PointTrackerDistance(new CityblockMetric(), windowSize);
    tracker->setPointTracker(pTracker);
    updateTrackerInfo();
}

void WindowAnotationManager::on_actionSSD_triggered()
{
    int windowSize = QInputDialog::getInt(this, "Window size", "Window size:", 21, 1, 100, 2);
    PointTrackerBase *pTracker = new PointTrackerDistance(new SumOfSquareDifferences(), windowSize);
    tracker->setPointTracker(pTracker);
    updateTrackerInfo();
}

void WindowAnotationManager::on_actionOptical_flow_triggered()
{
    int (outlier) = QInputDialog::getInt(this, "Outlier distance", "Outlier distance:", 20, 5, 100, 1);
    PointTrackerBase *pTracker = new PointTrackerOpticalFlow(outlier);
    tracker->setPointTracker(pTracker);
    updateTrackerInfo();
}

void WindowAnotationManager::on_actionOptical_flow_with_neighbourhood_triggered()
{
    int (outlier) = QInputDialog::getInt(this, "Outlier distance", "Outlier distance:", 20, 5, 100, 1);
    int windowSize = QInputDialog::getInt(this, "Window size", "Window size:", 21, 1, 100, 2);
    int step = QInputDialog::getInt(this, "Step within window", "Step within window:", 5, 1, 10, 1);
    PointTrackerBase *pTracker = new PointTrackerNeighbourOpticalFlow(outlier, windowSize, step);
    tracker->setPointTracker(pTracker);
    updateTrackerInfo();
}

void WindowAnotationManager::on_actionCorrelation_triggered()
{
    int windowSize = QInputDialog::getInt(this, "Window size", "Window size:", 21, 1, 100, 2);
    PointTrackerBase *pTracker = new PointTrackerDistance(new CorrelationMetric(), windowSize);
    tracker->setPointTracker(pTracker);
    updateTrackerInfo();
}

void WindowAnotationManager::on_actionCosine_triggered()
{
    int windowSize = QInputDialog::getInt(this, "Window size", "Window size:", 21, 1, 100, 2);
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
    /*QString pcaPath = QInputDialog::getText(this, "Path to pca file", "Path to pca file", QLineEdit::Normal, "pca-shape2");
    if (pcaPath.isNull() || pcaPath.isEmpty()) return;
    PCA *pca = new PCA(dataDir + QDir::separator() + pcaPath);
    StatisticalShapeModel *model = new StatisticalShapeModel(pca);
    ShapeNormalizerBase* normalizer = new ShapeNormalizerIterativeStatisticalShape(model);
    tracker->getStrain()->SetShapeNormalizer(normalizer);
    updateTrackerInfo();*/
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

void WindowAnotationManager::on_btnShiftBeatLeft_clicked()
{
    VideoDataClip *clip = ui->widgetStrainVideo->getClip();
    if (!clip || clip->size() == 0) return;

    QAbstractItemModel *model = ui->listViewBeats->model();
    QModelIndex index = ui->listViewBeats->currentIndex();
    int row = index.row();
    if (row < 0) return;

    int value = clip->getMetadata()->beatIndicies[row];
    if (value > 0)
    {
        model->setData(index, value - 1);
        ui->widgetStrainVideo->display(value - 1);
    }
}

void WindowAnotationManager::on_btnShiftBeatRight_clicked()
{
    VideoDataClip *clip = ui->widgetStrainVideo->getClip();
    if (!clip || clip->size() == 0) return;

    QAbstractItemModel *model = ui->listViewBeats->model();
    QModelIndex index = ui->listViewBeats->currentIndex();
    int row = index.row();
    if (row < 0) return;

    int value = clip->getMetadata()->beatIndicies[row];
    if (value < clip->size() - 1)
    {
        model->setData(index, value + 1);
        ui->widgetStrainVideo->display(value + 1);
    }
}

void WindowAnotationManager::onBeatSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected)
{
    if (selected.empty()) return;
    if (selected.first().indexes().empty()) return;

    int selectedIndex = selected.first().indexes().first().row();
    int index = ui->widgetStrainVideo->getClip()->getMetadata()->beatIndicies[selectedIndex];
    qDebug() << "selected index" << index;
    ui->widgetStrainVideo->display(index);
}

void WindowAnotationManager::on_btnBeatDetect_clicked()
{
    VideoDataClip *clip = ui->widgetStrainVideo->getClip();
    if (!clip || clip->size() == 0) return;

    DialogBeatDetector dlg(this);
    if (dlg.exec() != QDialog::Accepted) return;

    QProgressDialog progress("Processing...", QString(), 0, clip->size());
    progress.setModal(Qt::WindowModal);
    progress.setMinimumDuration(0);

    BeatDetector detector(dlg.getSettings());
    clip->getMetadata()->beatIndicies = detector.detect(clip, false, &progress);
    updateBeatModel();
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
    int windowSize = QInputDialog::getInt(this, "Window size", "Window size:", 3, 1, 51, 2);
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

