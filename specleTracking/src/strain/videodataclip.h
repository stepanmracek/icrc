#ifndef VIDEODATACLIP_H
#define VIDEODATACLIP_H

#include <opencv2/opencv.hpp>
#include <QVector>

#include "videodatabase.h"
#include "linalg/common.h"
#include "linalg/serializableobject.h"

class VideoDataClipMetadata : public SerializableObject
{
public:
    QVector<int> beatIndicies;

    void serialize(const QString &path);
    void deserialize(const QString &path);
};

class VideoDataClip : public VideoDataBase
{
private:
    int currentIndex;

public:
    VideoDataClip();
    VideoDataClip(const QString &path);
    VideoDataClip(const QString &path, const QString &metadataPath);

    VideoDataClip getRange(int start, int end) const;
    void getBeatRange(int currentIndex, int &beatStart, int &beatEnd) const;

    VectorOfImages frames;
    VideoDataClipMetadata metadata;

    virtual void setIndex(int index);
    virtual bool getNextFrame(Mat8 &frame);
    virtual bool getFrame(Mat8 &frame, int index);
    virtual int size() const;
};

#endif // VIDEODATACLIP_H
