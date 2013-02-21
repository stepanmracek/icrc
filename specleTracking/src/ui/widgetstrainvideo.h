#ifndef WIDGETSTRAINVIDEO_H
#define WIDGETSTRAINVIDEO_H

#include <QWidget>
#include <QMap>
#include <QGraphicsScene>
#include <QDir>

#include "strain/strainstatistics.h"
#include "strain/videodataclip.h"
#include "strain/strain.h"
#include "strain/shapetracker.h"
#include "linalg/spline.h"
#include "linalg/serialization.h"
#include "ui/widgetresult.h"

namespace Ui {
class WidgetStrainVideo;
}

class WidgetStrainVideo : public QWidget
{
    Q_OBJECT

protected:
    VideoDataClip *clip;
    
public:
    explicit WidgetStrainVideo(QWidget *parent = 0);
    ~WidgetStrainVideo();

    WidgetResult *getWidgetResult();

    void setTracker(ShapeTracker *tracker);

    void setClip(VideoDataClip *clip);

    VideoDataClip *getClip()
    {
        return clip;
    }

    QMap<int, Points> shapes;
    int currentIndex;
    QString currentFilename;
    
private:
    Ui::WidgetStrainVideo *ui;

    Spline spline;

public slots:
    void serializeShapes(const QString &path);
    void serializeMetadata(const QString &path);
    void deserializeShapes(const QString &path);
    void load(const QString &path, const QString &filename);
    void setControlPoints(Points controlPoints, int shapeWidth);
    void setResultPoints(Points points);
    void display(int index);

private slots:
    void on_btnPrev_clicked();

    void on_btnNext_clicked();

    void on_horizontalSlider_valueChanged(int value);

signals:
    void displayIndexChanged(int index);
};

#endif // WIDGETSTRAINVIDEO_H
