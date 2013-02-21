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
#include "ui/dialoganotation.h"
#include "ui/dialogcreatecoordsystemradial.h"
#include "ui/dialogvideodataclipmetadata.h"

WindowAnotationManager::WindowAnotationManager(QString path, ShapeTracker *tracker, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::WindowAnotationManager),
    tracker(tracker)
{
    ui->setupUi(this);
    ui->widgetStrainVideo->setTracker(tracker);
    setDirectory(path);
}

WindowAnotationManager::~WindowAnotationManager()
{
    delete ui;
}

void WindowAnotationManager::closeEvent(QCloseEvent *event)
{
    ui->widgetStrainVideo->serializeShapes(path);
    ui->widgetStrainVideo->serializeMetadata(path);
}

void WindowAnotationManager::on_btnBrowse_clicked()
{
    QString selected = QFileDialog::getExistingDirectory(this, QString(), path);
    if (selected.isNull() || selected.isEmpty()) return;

    setDirectory(selected);
}

void WindowAnotationManager::setDirectory(QString path)
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
    ui->widgetStrainVideo->serializeShapes(path);
    ui->widgetStrainVideo->serializeMetadata(path);
    ui->widgetStrainVideo->load(path, fileName);
}
void WindowAnotationManager::on_btnAnotate_clicked()
{
    if (ui->widgetStrainVideo->getClip()->size() == 0) return;

    const Mat8 &frame = ui->widgetStrainVideo->getClip()->frames[ui->widgetStrainVideo->currentIndex];
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
    int limit = ui->widgetStrainVideo->getClip()->size() - 1;
    int currentIndex = ui->widgetStrainVideo->currentIndex;
    if (currentIndex == limit) return;

    VideoDataClip *clip = ui->widgetStrainVideo->getClip();

    if (!ui->widgetStrainVideo->shapes.contains(currentIndex))
    {
        QMessageBox msg(QMessageBox::Information, "Information", "Current frame is not anotated.");
        msg.exec();
        return;
    }

    int frames = QInputDialog::getInt(this, "Track", "Track for specific number of frames", 1, 1, 1000);
    QProgressDialog progress("Processing...", "Cancel", 0, frames);
    progress.setModal(Qt::WindowModal);

    for (int i = 0; i < frames; i++)
    {
        progress.setValue(i);
        //qDebug() << "Tracking" << i << "/" << frames;

        if (progress.wasCanceled()) break;

        // prepare previous shapes and previous frames
        VectorOfShapes prevShapes;
        VectorOfImages prevFrames;

        for (unsigned int j = 0; j < tracker->weights.size(); j++)
        {
            int prevIndex = currentIndex+i-j;
            //qDebug() << "  j:" << j << "prevIndex:" << prevIndex;
            if (prevIndex < currentIndex)
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

        int nextIndex = currentIndex+i+1;
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

    progress.setValue(frames);
}

void WindowAnotationManager::on_btnCoordSystem_clicked()
{
    VideoDataClip *clip = ui->widgetStrainVideo->getClip();
    if (!clip || clip->size() == 0) return;

    Mat8 frame = clip->frames[ui->widgetStrainVideo->currentIndex];
    DialogCreateCoordSystemRadial dlg(frame, clip->getMetadata()->getCoordSystem() , this);
    if (dlg.exec() == QDialog::Accepted)
    {
        clip->getMetadata()->getCoordSystem()->init(dlg.getNewCoordSystem());
        ui->widgetStrainVideo->display(ui->widgetStrainVideo->currentIndex);
    }
}

void WindowAnotationManager::on_btnMetadata_clicked()
{
    VideoDataClip *clip = ui->widgetStrainVideo->getClip();
    if (!clip || clip->size() == 0) return;

    DialogVideoDataClipMetadata dlg(clip->getMetadata());
    if (dlg.exec() == QDialog::Accepted)
    {
        clip->getMetadata()->beatIndicies = dlg.getMetadata()->beatIndicies;
    }
}

void WindowAnotationManager::on_btnStats_clicked()
{
    VideoDataClip *clip = ui->widgetStrainVideo->getClip();
    QMap<int, Points> &shapes = ui->widgetStrainVideo->shapes;
    int currentIndex = ui->widgetStrainVideo->currentIndex;

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
