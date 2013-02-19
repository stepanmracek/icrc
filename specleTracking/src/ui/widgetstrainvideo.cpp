#include "widgetstrainvideo.h"
#include "ui_widgetstrainvideo.h"

#include <QDebug>

#include "uiutils.h"

WidgetStrainVideo::WidgetStrainVideo(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetStrainVideo)
{
    ui->setupUi(this);
    currentIndex = 0;
}

WidgetStrainVideo::~WidgetStrainVideo()
{
    delete ui;
}

void WidgetStrainVideo::display(int index)
{
    Mat8 frame = clip.frames[index];
    QPixmap image = UIUtils::Mat8ToQPixmap(frame);
    ui->widgetResult->setImage(image);
    ui->lblIndex->setText(QString::number(index));
    if (shapes.contains(index))
        ui->widgetResult->setResultPoints(shapes[index], this->clip.metadata.coordSystem);
    emit displayIndexChanged(index);
}

void WidgetStrainVideo::on_btnPrev_clicked()
{
    if (currentIndex <= 0) return;

    ui->horizontalSlider->setValue(currentIndex - 1);
}

void WidgetStrainVideo::on_btnNext_clicked()
{
    if (currentIndex >= clip.size()-2) return;

    ui->horizontalSlider->setValue(currentIndex + 1);
}

void WidgetStrainVideo::on_horizontalSlider_valueChanged(int value)
{
    if (clip.size() == 0) return;
    if (value >= clip.size()) return;

    Mat8 frame = clip.frames[value];
    QPixmap image = UIUtils::Mat8ToQPixmap(frame);
    ui->widgetResult->setImage(image);
    currentIndex = value;

    if (shapes.contains(currentIndex))
    {
        ui->widgetResult->setResultPoints(Points(shapes[currentIndex]), this->clip.metadata.coordSystem);
    }
    else
    {
        ui->widgetResult->setResultPoints(Points(), this->clip.metadata.coordSystem);
    }
}

void WidgetStrainVideo::load(const QString &path, const QString &filename)
{
    currentIndex = 0;
    QString fullPath = path + QDir::separator() + filename;
    QString fullMetadataPath = fullPath + "_metadata";
    currentFilename = filename;
    VideoDataClip loadedClip(fullPath, fullMetadataPath);
    setClip(loadedClip);

    QPixmap image = UIUtils::Mat8ToQPixmap(clip.frames[0]);
    ui->widgetResult->setImage(image);

    shapes.clear();
    deserializeShapes(path);
    ui->horizontalSlider->setValue(0);
    on_horizontalSlider_valueChanged(0);
}

void WidgetStrainVideo::serializeShapes(const QString &path)
{
    if (clip.size() == 0 || shapes.size() == 0) return;

    qDebug() << "Saving shapes for" << currentFilename;
    QString fullPath = path + QDir::separator() + currentFilename + "_shapemap";
    Serialization::serialize(shapes, fullPath.toStdString().c_str());
}

void WidgetStrainVideo::serializeMetadata(const QString &path)
{
    if (clip.size() == 0) return;

    qDebug() << "Saving metadata for" << currentFilename;
    QString fullPath = path + QDir::separator() + currentFilename + "_metadata";
    clip.metadata.serialize(fullPath);
}

void WidgetStrainVideo::deserializeShapes(const QString &path)
{
    QString fullpath = path + QDir::separator() + currentFilename + "_shapemap";
    qDebug() << "Loading shapes for" << currentFilename;

    QFileInfo fInfo(fullpath);
    if (!fInfo.exists()) return;
    shapes = Serialization::readShapeMap(fullpath.toStdString().c_str());
}

WidgetResult *WidgetStrainVideo::getWidgetResult()
{
    return ui->widgetResult;
}

void WidgetStrainVideo::setTracker(ShapeTracker *tracker)
{
    ui->widgetResult->setTracker(tracker);
}

void WidgetStrainVideo::setClip(VideoDataClip &clip)
{
    this->clip = clip;
    ui->horizontalSlider->setMinimum(0);
    ui->horizontalSlider->setMaximum(clip.size()-1);
}

void WidgetStrainVideo::setControlPoints(Points controlPoints, int shapeWidth)
{
    ui->widgetResult->setControlPoints(controlPoints, shapeWidth, this->clip.metadata.coordSystem);
    shapes[currentIndex] = ui->widgetResult->getResultPoints();
}

void WidgetStrainVideo::setResultPoints(Points points)
{
    ui->widgetResult->setResultPoints(points, this->clip.metadata.coordSystem);
    shapes[currentIndex] = points;
}
