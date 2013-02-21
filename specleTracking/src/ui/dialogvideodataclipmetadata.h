#ifndef DIALOGVIDEODATACLIPMETADATA_H
#define DIALOGVIDEODATACLIPMETADATA_H

#include <QDialog>

#include "strain/videodataclip.h"
#include "modellistofints.h"

namespace Ui {
class DialogVideoDataClipMetadata;
}

class DialogVideoDataClipMetadata : public QDialog
{
    Q_OBJECT
    
private:
    VideoDataClipMetadata *metadata;

public:
    explicit DialogVideoDataClipMetadata(VideoDataClipMetadata *metadata, QWidget *parent = 0);
    ~DialogVideoDataClipMetadata();

    VideoDataClipMetadata *getMetadata() { return metadata; }
    
private slots:
    void on_btnAdd_clicked();

private:
    ModelListOfInts *model;
    Ui::DialogVideoDataClipMetadata *ui;
};

#endif // DIALOGVIDEODATACLIPMETADATA_H
