#ifndef VIDEODATABASE_H
#define VIDEODATABASE_H

#include <QObject>
#include "linalg/common.h"

class VideoDataBase : public QObject
{
    Q_OBJECT

public:
    VideoDataBase(QObject *parent = 0) : QObject(parent) { }

    virtual int index() const = 0;
    virtual void setIndex(int index) = 0;
    virtual bool getNextFrame(Mat8 &frame) = 0;
    virtual bool getFrame(Mat8 &frame, int index) = 0;
    virtual int size() const = 0;
};

#endif // VIDEODATABASE_H
