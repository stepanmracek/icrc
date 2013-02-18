#include "widgetanotationmanager.h"
#include "ui_widgetanotationmanager.h"

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
#include "ui/dialoganotation.h"
#include "ui/dialogcreatecoordsystemradial.h"
#include "ui/dialogvideodataclipmetadata.h"

WidgetAnotationManager::WidgetAnotationManager(QString path, ShapeTracker *tracker, QWidget *parent) :
    tracker(tracker),
    QWidget(parent),
    ui(new Ui::WidgetAnotationManager)
{
    ui->setupUi(this);
    ui->widgetStrainVideo->setTracker(tracker);
    setDirectory(path);
}

WidgetAnotationManager::~WidgetAnotationManager()
{
    delete ui;
}

void WidgetAnotationManager::on_btnBrowse_clicked()
{
    QString selected = QFileDialog::getExistingDirectory(this, QString(), path);
    if (selected.isNull() || selected.isEmpty()) return;

    setDirectory(selected);
}

void WidgetAnotationManager::setDirectory(QString path)
{
    this->path = path;
    ui->lineEdit->setText(path);
    ui->listFiles->clear();

    QDir dir(path);
    if (!dir.exists()) return;

    QStringList filter; filter << "*.wmv" << "*.WMV";
    QStringList files = dir.entryList(filter, QDir::Files | QDir::NoDotAndDotDot, QDir::Name);

    ui->listFiles->addItems(files);

    if (files.count() > 0)
    {
        ui->listFiles->setCurrentRow(0);
        on_btnLoad_clicked();
    }
}

void WidgetAnotationManager::on_btnLoad_clicked()
{
    QList<QListWidgetItem*> items = ui->listFiles->selectedItems();
    if (items.count() == 0) return;

    loadFile(items[0]->text());
}

void WidgetAnotationManager::loadFile(QString fileName)
{
    QString fullPath = path + QDir::separator() + fileName;
    QFileInfo f(fullPath);
    if (!f.exists()) return;

    qDebug() << "Loading" << fileName;
    ui->widgetStrainVideo->serializeShapes(path);
    ui->widgetStrainVideo->serializeMetadata(path);
    ui->widgetStrainVideo->load(path, fileName);
}

void WidgetAnotationManager::on_listFiles_itemDoubleClicked(QListWidgetItem *)
{
    on_btnLoad_clicked();
}

void WidgetAnotationManager::on_btnTrack_clicked()
{
    int limit = ui->widgetStrainVideo->getClip().size() - 1;
    int currentIndex = ui->widgetStrainVideo->currentIndex;
    if (currentIndex == limit) return;

    if (!ui->widgetStrainVideo->shapes.contains(currentIndex))
    {
        qDebug() << "Current frame is not anotated!";
        return;
    }

    int frames = QInputDialog::getInt(this, "Track", "Track for specific number of frames", 1, 1, 1000);
    QProgressDialog progress("Processing...", "Cancel", 0, frames);
    progress.setModal(Qt::WindowModal);

    for (int i = 0; i < frames; i++)
    {
        progress.setValue(i);
        qDebug() << "Tracking" << i << "/" << frames;

        if (progress.wasCanceled()) break;

        // prepare previous shapes and previous frames
        VectorOfShapes prevShapes;
        VectorOfImages prevFrames;

        for (unsigned int j = 0; j < tracker->weights.size(); j++)
        {
            int prevIndex = currentIndex+i-j;
            qDebug() << "  j:" << j << "prevIndex:" << prevIndex;
            if (prevIndex < currentIndex)
            {
                qDebug() << "  prevIndex < currentIndex";
                break;
            }
            if (prevIndex == limit)
            {
                qDebug() << "  prevIndex == limit";
                break;
            }

            prevShapes.push_back(ui->widgetStrainVideo->shapes[prevIndex]);
            prevFrames.push_back(ui->widgetStrainVideo->getClip().frames[prevIndex]);
        }
        qDebug() << "  prevShape/prevFrames size:" << prevShapes.size() << prevFrames.size();
        if (prevShapes.size() == 0) break;

        int nextIndex = currentIndex+i+1;
        qDebug() << "  nextIndex:" << nextIndex;
        if (nextIndex == limit) break;
        Mat8 nextFrame = ui->widgetStrainVideo->getClip().frames[nextIndex];

        qDebug() << "  reversing prevShapes and prevFrames";
        std::reverse(prevShapes.begin(), prevShapes.end());
        std::reverse(prevFrames.begin(), prevFrames.end());
        qDebug() << "  tracking";
        Points nextShape = tracker->track(prevFrames, prevShapes, nextFrame);
        ui->widgetStrainVideo->shapes[nextIndex] = nextShape;
    }

    progress.setValue(frames);
}

void WidgetAnotationManager::closeEvent(QCloseEvent *event)
{
    ui->widgetStrainVideo->serializeShapes(path);
    ui->widgetStrainVideo->serializeMetadata(path);
}

void WidgetAnotationManager::on_widgetResult_noImage()
{
    QMessageBox msg(QMessageBox::Information, "Information", "Load the image first");
    msg.exec();
}

void WidgetAnotationManager::on_btnStats_clicked()
{
    // determine current beat
    const VideoDataClip &clip = ui->widgetStrainVideo->getClip();
    QMap<int, Points> &shapes = ui->widgetStrainVideo->shapes;
    if (clip.metadata.beatIndicies.size() == 0)
    {
        qDebug() << "Clip metadata not set";
        return;
    }
    int beatStart;
    int beatEnd;
    int currentIndex = ui->widgetStrainVideo->currentIndex;
    clip.getBeatRange(currentIndex, beatStart, beatEnd);
    qDebug() << "beat range:" << beatStart << beatEnd;

    // check if all shapes within current beat are present
    bool canProceed = true;
    for (int i = beatStart; i < beatEnd; i++)
    {
        if (!shapes.contains(i))
        {
            canProceed = false;
            break;
        }
    }

    if (!canProceed)
    {
        QMessageBox msg(QMessageBox::Information, "Information", "Not all shapes within beat are defined.");
        msg.exec();
        return;
    }

    VideoDataClip subClip = clip.getRange(beatStart, beatEnd);
    qDebug() << "subClip.size()" << subClip.size();
    VectorOfShapes vectorOfShapes;
    QMap<int, Points> subShapesMap;
    for (int i = beatStart; i < beatEnd; i++)
    {
        vectorOfShapes.push_back(shapes[i]);
        subShapesMap[i-beatStart] = shapes[i];
    }

    StrainStatistics stats(tracker->strain, vectorOfShapes);
    DialogStrainStatistics dlgStats;
    dlgStats.SetData(&stats, tracker, ui->widgetStrainVideo->getClip(), ui->widgetStrainVideo->shapes);
    dlgStats.exec();
}

void WidgetAnotationManager::on_btnAnotate_clicked()
{
    if (ui->widgetStrainVideo->getClip().size() == 0) return;

    const Mat8 &frame = ui->widgetStrainVideo->getClip().frames[ui->widgetStrainVideo->currentIndex];
    QPixmap image = UIUtils::Mat8ToQPixmap(frame);
    DialogAnotation dlgAnotation(this);
    dlgAnotation.setImage(image);
    int dlgResult = dlgAnotation.exec();

    if (dlgResult == QDialog::Accepted)
    {
        ui->widgetStrainVideo->setControlPoints(dlgAnotation.getControlPoints(), dlgAnotation.getShapeWidth());
    }
}

void WidgetAnotationManager::on_btnCoordSystem_clicked()
{
    if (ui->widgetStrainVideo->getClip().size() == 0) return;

    Mat8 frame = ui->widgetStrainVideo->getClip().frames[ui->widgetStrainVideo->currentIndex];
    DialogCreateCoordSystemRadial dlg(frame, (CoordSystemRadial*)(tracker->coordSystem), this);
    if (dlg.exec() == QDialog::Accepted)
    {
        tracker->coordSystem = dlg.getNewCoordSystem();
        ui->widgetStrainVideo->display(ui->widgetStrainVideo->currentIndex);
    }
}

void WidgetAnotationManager::on_btnMetadata_clicked()
{
    if (ui->widgetStrainVideo->getClip().size() == 0) return;

    DialogVideoDataClipMetadata dlg(ui->widgetStrainVideo->getClip().metadata);
    if (dlg.exec() == QDialog::Accepted)
    {
        ui->widgetStrainVideo->getClip().metadata = dlg.metadata;
    }
}
