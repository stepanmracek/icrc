#ifndef WIDGETSTRAINVIDEO_H
#define WIDGETSTRAINVIDEO_H

#include <QWidget>
#include <QMap>
#include <QGraphicsScene>
#include <QDir>
#include <QString>

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

    VideoDataClip *getClip() { return clip; }
    int getCurrentIndex() { return currentIndex; }
    QString getCurrentFilename() { return currentFilename; }

    QMap<int, Points> shapes;
    
private:
    Ui::WidgetStrainVideo *ui;
    Spline spline;
    int currentIndex;
    QString currentFilename;

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

    void on_btnPrevBeat_clicked();

    void on_btnNextBeat_clicked();

signals:
    void displayIndexChanged(int index);
};

#endif // WIDGETSTRAINVIDEO_H
