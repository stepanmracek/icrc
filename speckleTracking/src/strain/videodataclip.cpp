#include "videodataclip.h"

VideoDataClip::VideoDataClip(const QString &path, QObject *parent) : VideoDataBase(parent)
{
    metadata = new VideoDataClipMetadata(this);

    cv::VideoCapture capture = cv::VideoCapture(path.toStdString());
    cv::Mat rawFrame;
    while (capture.read(rawFrame))
    {
        Mat8 frame;
        cv::cvtColor(rawFrame, frame, cv::COLOR_BGR2GRAY);
        frames.push_back(frame);
    }
}

VideoDataClip::VideoDataClip(const QString &path, const QString &metadataPath, QObject *parent) : VideoDataBase(parent)
{
    metadata = new VideoDataClipMetadata(this);

    cv::VideoCapture capture = cv::VideoCapture(path.toStdString());
    assert(capture.isOpened());
    cv::Mat rawFrame;
    while (capture.read(rawFrame))
    {
        Mat8 frame;
        cv::cvtColor(rawFrame, frame, cv::COLOR_BGR2GRAY);
        frames.push_back(frame);
    }

    metadata->deserialize(metadataPath);
}

VideoDataClip::VideoDataClip(QObject *parent) : VideoDataBase(parent)
{
    metadata = new VideoDataClipMetadata(this);
}

bool VideoDataClip::getNextFrame(Mat8 &frame)
{
    if (currentIndex >= size()) return false;

    frames[currentIndex].copyTo(frame);
    currentIndex++;
    return true;
}

bool VideoDataClip::getFrame(Mat8 &frame, int index)
{
    setIndex(index);
    return getNextFrame(frame);
}

int VideoDataClip::size() const
{
    return frames.size();
}

void VideoDataClip::setIndex(int index)
{
    currentIndex = index;
}

void VideoDataClip::getRange(int start, int end, VideoDataClip *outClip) const
{
    outClip->frames.clear();

    for (int i = 0; i < frames.size(); i++)
    {
        if (i >= start && i < end)
        {
            outClip->frames.push_back(frames[i]);
        }
    }
    for (int i = 0; i < metadata->beatIndicies.size(); i++)
    {
        int beat = metadata->beatIndicies[i];
        if (beat >= start && beat < end)
        {
            outClip->metadata->beatIndicies.push_back(beat);
        }
    }

    outClip->getMetadata()->getCoordSystem()->init(metadata->getCoordSystem());
}

void VideoDataClip::getBeatRange(int currentIndex, int &beatStart, int &beatEnd) const
{
    beatStart = 0;
    beatEnd = size() - 1;

    for (int i = 0; i < metadata->beatIndicies.size(); i++)
    {
        int beatIndex = metadata->beatIndicies[i];
        if (beatIndex <= currentIndex && beatIndex >= beatStart)
        {
            beatStart = beatIndex;
        }
        if (beatIndex > currentIndex && beatIndex <= beatEnd)
        {
            beatEnd = beatIndex;
        }
    }
}

char *VideoDataClip::getSubClip(int index, QMap<int, Points> &shapes,
                                VideoDataClip *outSubCLip, VectorOfShapes &outSubShapes, QMap<int, Points> &outSubShapesMap)
{
    // determine current beat
    if (metadata->beatIndicies.size() == 0)
    {
        return "Clip metadata are not set.";
    }
    int beatStart;
    int beatEnd;
    getBeatRange(index, beatStart, beatEnd);

    // check if all shapes within current beat are present
    for (int i = beatStart; i < beatEnd; i++)
    {
        if (!shapes.contains(i))
        {
            return "Not all shapes within beat are defined.";
        }
    }

    getRange(beatStart, beatEnd, outSubCLip);
    outSubShapes.clear();
    outSubShapesMap.clear();
    for (int i = beatStart; i < beatEnd; i++)
    {
        outSubShapes.push_back(shapes[i]);
        outSubShapesMap[i-beatStart] = shapes[i];
    }

    return 0;
}

void VideoDataClipMetadata::deserialize(const QString &path)
{
    cv::FileStorage storage(path.toStdString(), cv::FileStorage::READ);
    if (!storage.isOpened())
    {
        return;
    }
    cv::FileNode node = storage["beatIndicies"];
    for (cv::FileNodeIterator it = node.begin(); it != node.end(); ++it)
    {
        int i;
        (*it) >> i;
        beatIndicies << i;
    }

    VectorF centerVec;
    cv::FileNodeIterator it = storage["center"].begin();
    cv::FileNodeIterator end = storage["center"].end();
    for(; it != end; ++it)
    {
        float val = (float)(*it);
        centerVec.push_back(val);
    }
    P center(centerVec[0], centerVec[1]);

    float startDistance = (float)storage["startDistance"];
    float endDistance = (float)storage["endDistance"];
    float angleStart = (float)storage["angleStart"];
    float angleEnd = (float)storage["angleEnd"];
    int resultMatCols = (int)storage["resultMatCols"];
    int resultMatRows = (int)storage["resultMatRows"];
    coordSystem->init(center, startDistance, endDistance, angleStart, angleEnd, resultMatCols, resultMatRows);
}

void VideoDataClipMetadata::serialize(const QString &path)
{
    cv::FileStorage storage(path.toStdString(), cv::FileStorage::WRITE);
    if (!storage.isOpened()) return;
    storage << "beatIndicies" << "[";
    foreach(int i, beatIndicies)
    {
        storage << i;
    }
    storage << "]";

    //void CoordSystemRadial::init(P center, float startDistance, float endDistance, float angleStart, float angleEnd, int resultMatCols, int resultMatRows)
    storage << "center" << coordSystem->center;
    storage << "startDistance" << coordSystem->startDistance;
    storage << "endDistance" << coordSystem->endDistance;
    storage << "angleStart" << coordSystem->angleStart;
    storage << "angleEnd" << coordSystem->angleEnd;
    storage << "resultMatCols" << coordSystem->resultMatCols;
    storage << "resultMatRows" << coordSystem->resultMatRows;
}
