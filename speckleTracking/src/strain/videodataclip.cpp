#include "videodataclip.h"

#include <QDebug>

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

VideoDataClip::VideoDataClip(const QString &path, const QString &metadataPath, QProgressDialog *progressDlg, QObject *parent) : VideoDataBase(parent)
{
    metadata = new VideoDataClipMetadata(this);

    cv::VideoCapture capture = cv::VideoCapture(path.toStdString());
    assert(capture.isOpened());

    int frameCount = capture.get(CV_CAP_PROP_FRAME_COUNT);
    if (progressDlg) progressDlg->setMaximum(frameCount);
    frames.reserve(frameCount);

    cv::Mat rawFrame;
    for (int frameIndex = 0; frameIndex < frameCount; frameIndex++)
    {
        capture.read(rawFrame);
        if (rawFrame.rows == 0 || rawFrame.cols == 0) continue;

        Mat8 frame;
        cv::cvtColor(rawFrame, frame, cv::COLOR_BGR2GRAY);
        frames.push_back(frame);

        if (progressDlg) progressDlg->setValue(frameIndex);
    }
    if (progressDlg) progressDlg->setValue(frameCount);

    cv::FileStorage metadataStorage(metadataPath.toStdString(), cv::FileStorage::READ);
    metadata->deserialize(metadataStorage);
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

int VideoDataClip::index() const
{
    return currentIndex;
}

void VideoDataClip::setIndex(int index)
{
    currentIndex = index;
}

void VideoDataClip::getRange(int start, int end, VideoDataClip *outClip) const
{
    outClip->frames.clear();

    for (unsigned int i = 0; i < frames.size(); i++)
    {
        if ((int)i >= start && (int)i < end)
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

    outClip->getMetadata()->setCoordSystem(metadata->getCoordSystem()->clone());
}

void VideoDataClip::getBeatRange(int currentIndex, int &beatStart, int &beatEnd) const
{
    if (metadata->beatIndicies.count() == 0)
    {
        beatStart = -1; beatEnd = -1;
        return;
    }
    if (currentIndex < metadata->beatIndicies.first())
    {
        beatStart = -1; beatEnd = metadata->beatIndicies.first();
        return;
    }
    if (currentIndex >= metadata->beatIndicies.last())
    {
        beatStart = metadata->beatIndicies.last();
        beatEnd = -1;
        return;
    }

    beatStart = 0;
    beatEnd = size() - 1;
    foreach (int beatIndex, metadata->beatIndicies)
    {
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

QString VideoDataClip::getSubClip(int index, QMap<int, Points> &shapes,
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

    return QString();
}

VideoDataClipMetadata::VideoDataClipMetadata(QObject *parent) : SerializableObject(parent)
{
    coordSystem = new CoordSystemROI(this);
}

void VideoDataClipMetadata::deserialize(cv::FileStorage &storage)
{
    if (!storage.isOpened()) return;

    cv::FileNode node = storage["beatIndicies"];
    for (cv::FileNodeIterator it = node.begin(); it != node.end(); ++it)
    {
        int i;
        (*it) >> i;
        beatIndicies << i;
    }

    rawShapes.clear();
    QVector<int> rawShapesKeys;
    cv::FileNode rawShapesKeysNode = storage["rawShapesKeys"];
    for (cv::FileNodeIterator it = rawShapesKeysNode.begin(); it != rawShapesKeysNode.end(); ++it)
    {
        int i;
        (*it) >> i;
        rawShapesKeys << i;
    }

    cv::FileNode rawShapesValuesNode = storage["rawShapesValues"];
    int index = 0;
    for (cv::FileNodeIterator it = rawShapesValuesNode.begin(); it != rawShapesValuesNode.end(); ++it)
    {
        MatF m;
        (*it) >> m;
        Points shape = Common::matFToPoints(m);
        rawShapes[rawShapesKeys[index]] = shape;

        index++;
    }

    CoordSystemBase::Types type = (CoordSystemBase::Types)(int)(storage["coordSystemType"]);
    if (coordSystem) delete coordSystem;
    if (type == CoordSystemBase::TypeRadial)
    {
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

        coordSystem = new CoordSystemRadial(center, startDistance, endDistance, angleStart, angleEnd,
                                            resultMatCols, resultMatRows, this);
    }
    else if (type == CoordSystemBase::TypeROI)
    {
        int x = (int)storage["x"];
        int y = (int)storage["y"];
        int width = (int)storage["width"];
        int height = (int)storage["height"];

        coordSystem = new CoordSystemROI(cv::Rect(x, y, width, height), this);
    }
}

void VideoDataClipMetadata::serialize(cv::FileStorage &storage) const
{
    storage << "beatIndicies" << "[";
    foreach(int i, beatIndicies)
    {
        storage << i;
    }
    storage << "]";

    storage << "rawShapesKeys" << "[";
    QList<int> keys = rawShapes.keys();
    foreach(int key, keys)
    {
        storage << key;
    }
    storage << "]";

    storage << "rawShapesValues" << "[";
    foreach(int key, keys)
    {
        const Points &shape = rawShapes[key];
        MatF m = Common::pointsToMatF(shape);
        storage << m;
    }
    storage << "]";

    storage << "coordSystemType" << coordSystem->type();
    switch(coordSystem->type())
    {
    case CoordSystemBase::TypeRadial:
    {
        CoordSystemRadial *cr = (CoordSystemRadial*)coordSystem;

        storage << "center" << cr->center;
        storage << "startDistance" << cr->startDistance;
        storage << "endDistance" << cr->endDistance;
        storage << "angleStart" << cr->angleStart;
        storage << "angleEnd" << cr->angleEnd;
        storage << "resultMatCols" << cr->resultMatCols;
        storage << "resultMatRows" << cr->resultMatRows;
        break;
    }
    case CoordSystemBase::TypeROI:
    {
        CoordSystemROI *roi = (CoordSystemROI*)coordSystem;

        storage << "x" << roi->roi.x;
        storage << "y" << roi->roi.y;
        storage << "width" << roi->roi.width;
        storage << "height" << roi->roi.height;
        break;
    }
    }
}

void VideoDataClipMetadata::setCoordSystem(CoordSystemBase *coordSystem)
{
    if (this->coordSystem) delete this->coordSystem;
    this->coordSystem = coordSystem;
    this->coordSystem->setParent(this);
}
