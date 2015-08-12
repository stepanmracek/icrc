#include "widgetstrainvideo.h"
#include "ui_widgetstrainvideo.h"

#include <QDebug>
#include <QProgressDialog>

#include "uiutils.h"

WidgetStrainVideo::WidgetStrainVideo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetStrainVideo)
{
    clip = 0;
    ui->setupUi(this);
    currentIndex = 0;
}

WidgetStrainVideo::~WidgetStrainVideo()
{
    delete ui;
}

void WidgetStrainVideo::display(int index)
{
    ui->horizontalSlider->setValue(index);
}

void WidgetStrainVideo::on_btnPrev_clicked()
{
    if (!clip) return;
    if (currentIndex <= 0) return;

    ui->horizontalSlider->setValue(currentIndex - 1);
}

void WidgetStrainVideo::on_btnNext_clicked()
{
    if (!clip) return;
    if (currentIndex >= clip->size()-2) return;

    ui->horizontalSlider->setValue(currentIndex + 1);
}

void WidgetStrainVideo::on_btnPrevBeat_clicked()
{
    if (!clip) return;
    int beatStart; int beatEnd;
    clip->getBeatRange(currentIndex, beatStart, beatEnd);

    if (beatStart == currentIndex)
    {
        int i = clip->getMetadata()->beatIndicies.indexOf(beatStart);
        if (i <= 0) return;

        ui->horizontalSlider->setValue(clip->getMetadata()->beatIndicies[i-1]);
    }
    else
    {
        ui->horizontalSlider->setValue(beatStart);
    }
}

void WidgetStrainVideo::on_btnNextBeat_clicked()
{
    if (!clip) return;
    int beatStart; int beatEnd;
    clip->getBeatRange(currentIndex, beatStart, beatEnd);
    if (beatEnd != -1)
        ui->horizontalSlider->setValue(beatEnd);
}

void WidgetStrainVideo::on_horizontalSlider_valueChanged(int value)
{
    if (!clip) {
        qDebug() << "on_horizontalSlider_valueChanged: no clip";
        return;
    }
    if (clip->size() == 0) {
        qDebug() << "on_horizontalSlider_valueChanged: empty clip";
        return;
    }
    if (value >= clip->size()) {
        qDebug() << "on_horizontalSlider_valueChanged: requested value exceeds clip size";
        return;
    }

    Mat8 &frame = clip->frames[value];
    QPixmap image = UIUtils::Mat8ToQPixmap(frame);
    ui->widgetResult->setImage(image);
    currentIndex = value;
    ui->lblIndex->setText(QString::number(value));

    if (shapes.contains(currentIndex))
    {
        ui->widgetResult->setResultPoints(Points(shapes[currentIndex]), clip->getMetadata()->getCoordSystem());
    }
    else
    {
        ui->widgetResult->setResultPoints(Points(), clip->getMetadata()->getCoordSystem());
    }

    emit displayIndexChanged(value);
}

void WidgetStrainVideo::unload()
{
    currentIndex = 0;
    currentFilename = "";
    clip = 0;
}

void WidgetStrainVideo::load(const QString &path, const QString &filename)
{
    currentIndex = 0;
    QString fullPath = path + QDir::separator() + filename;
    QString fullMetadataPath = fullPath + "_metadata";
    currentFilename = filename;

    QProgressDialog progDlg("Loading video", QString(), 0, 500, this);
    progDlg.setWindowModality(Qt::WindowModal);
    progDlg.setMinimumDuration(100);
    VideoDataClip *loadedClip = new VideoDataClip(fullPath, fullMetadataPath, &progDlg);
    setClip(loadedClip);

    QPixmap image = UIUtils::Mat8ToQPixmap(clip->frames[0]);
    ui->widgetResult->setImage(image);

    shapes.clear();
    deserializeShapes(path);
    qDebug() << "shapes loaded";
    ui->horizontalSlider->setValue(0);
    on_horizontalSlider_valueChanged(0);
}

void WidgetStrainVideo::serializeShapes(const QString &path)
{
    if (!clip || clip->size() == 0 || shapes.size() == 0) return;

    qDebug() << "Saving shapes for" << currentFilename;
    QString fullPath = path + QDir::separator() + currentFilename + "_shapemap";
    Serialization::serialize(shapes, fullPath);
}

void WidgetStrainVideo::serializeMetadata(const QString &path)
{
    if (!clip || clip->size() == 0) return;

    qDebug() << "Saving metadata for" << currentFilename;
    QString fullPath = path + QDir::separator() + currentFilename + "_metadata";
    cv::FileStorage storage(fullPath.toStdString(), cv::FileStorage::WRITE);
    clip->getMetadata()->serialize(storage);
}

void WidgetStrainVideo::deserializeShapes(const QString &path)
{
    QString fullpath = path + QDir::separator() + currentFilename + "_shapemap";
    qDebug() << "Loading shapes for" << currentFilename;

    QFileInfo fInfo(fullpath);
    if (!fInfo.exists()) return;
    shapes = Serialization::readShapeMap(fullpath);
}

WidgetResult *WidgetStrainVideo::getWidgetResult()
{
    return ui->widgetResult;
}

void WidgetStrainVideo::setTracker(ShapeTracker *tracker)
{
    ui->widgetResult->setTracker(tracker);
}

void WidgetStrainVideo::setClip(VideoDataClip *clip)
{
    if (clip) delete this->clip;
    this->clip = clip;
    clip->setParent(this);
    ui->horizontalSlider->setMinimum(0);
    ui->horizontalSlider->setMaximum(clip->size()-1);
}

void WidgetStrainVideo::setControlPoints(Points controlPoints, int shapeWidth)
{
    ui->widgetResult->setControlPoints(controlPoints, shapeWidth, clip->getMetadata()->getCoordSystem());
    shapes[currentIndex] = ui->widgetResult->getResultPoints();
}

void WidgetStrainVideo::setResultPoints(Points points)
{
    ui->widgetResult->setResultPoints(points, clip->getMetadata()->getCoordSystem());
    shapes[currentIndex] = points;
}
