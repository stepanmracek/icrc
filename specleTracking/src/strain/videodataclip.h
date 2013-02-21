#ifndef VIDEODATACLIP_H
#define VIDEODATACLIP_H

#include <opencv2/opencv.hpp>
#include <QVector>
#include <QMap>

#include "coordsystem.h"
#include "videodatabase.h"
#include "linalg/common.h"
#include "linalg/serializableobject.h"

class VideoDataClipMetadata : public SerializableObject
{
    Q_OBJECT

private:
    CoordSystemRadial *coordSystem;

public:
    VideoDataClipMetadata(QObject *parent) : SerializableObject(parent)
    {
        coordSystem = new CoordSystemRadial(this);
    }

    QVector<int> beatIndicies;

    CoordSystemRadial *getCoordSystem() { return coordSystem; }

    void serialize(const QString &path);
    void deserialize(const QString &path);
};

class VideoDataClip : public VideoDataBase
{
    Q_OBJECT

private:
    int currentIndex;

    VideoDataClipMetadata *metadata;

public:
    VideoDataClip(QObject *parent = 0);
    VideoDataClip(const QString &path, QObject *parent = 0);
    VideoDataClip(const QString &path, const QString &metadataPath, QObject *parent = 0);

    VectorOfImages frames;
    VideoDataClipMetadata *getMetadata() { return metadata; }

    void getRange(int start, int end, VideoDataClip *outClip) const;
    void getBeatRange(int currentIndex, int &beatStart, int &beatEnd) const;

    char *getSubClip(int index, QMap<int, Points> &shapes,
                     VideoDataClip *outSubCLip, VectorOfShapes &outSubShapes, QMap<int, Points> &outSubShapesMap);

    virtual void setIndex(int index);
    virtual bool getNextFrame(Mat8 &frame);
    virtual bool getFrame(Mat8 &frame, int index);
    virtual int size() const;
};

#endif // VIDEODATACLIP_H
