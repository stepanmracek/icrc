#include "videodataclip.h"

VideoDataClip::VideoDataClip(const QString &path)
{
    cv::VideoCapture capture = cv::VideoCapture(path.toUtf8().data());
    cv::Mat rawFrame;
    while (capture.read(rawFrame))
    {
        Mat8 frame;
        cv::cvtColor(rawFrame, frame, cv::COLOR_BGR2GRAY);
        frames.push_back(frame);
    }
}

VideoDataClip::VideoDataClip(const QString &path, const QString &metadataPath)
{
    cv::VideoCapture capture = cv::VideoCapture(path.toUtf8().data());
    cv::Mat rawFrame;
    while (capture.read(rawFrame))
    {
        Mat8 frame;
        cv::cvtColor(rawFrame, frame, cv::COLOR_BGR2GRAY);
        frames.push_back(frame);
    }

    metadata.deserialize(metadataPath);
}

VideoDataClip::VideoDataClip()
{

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

VideoDataClip VideoDataClip::getRange(int start, int end) const
{
    VideoDataClip clip;
    for (int i = 0; i < frames.size(); i++)
    {
        if (i >= start && i < end)
        {
            clip.frames.push_back(frames[i]);
        }
    }
    for (int i = 0; i < metadata.beatIndicies.size(); i++)
    {
        int beat = metadata.beatIndicies[i];
        if (beat >= start && beat < end)
        {
            clip.metadata.beatIndicies.push_back(beat);
        }
    }
    return clip;
}

void VideoDataClip::getBeatRange(int currentIndex, int &beatStart, int &beatEnd) const
{
    beatStart = 0;
    beatEnd = size() - 1;

    for (int i = 0; i < metadata.beatIndicies.size(); i++)
    {
        int beatIndex = metadata.beatIndicies[i];
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

void VideoDataClipMetadata::deserialize(const QString &path)
{
    cv::FileStorage storage(path.toUtf8().data(), cv::FileStorage::READ);
    cv::FileNode node = storage["beatIndicies"];
    for (cv::FileNodeIterator it = node.begin(); it != node.end(); ++it)
    {
        int i;
        (*it) >> i;
        beatIndicies << i;
    }
}

void VideoDataClipMetadata::serialize(const QString &path)
{
    cv::FileStorage storage(path.toUtf8().data(), cv::FileStorage::WRITE);
    if (!storage.isOpened()) return;
    storage << "beatIndicies" << "[";
    foreach(int i, beatIndicies)
    {
        storage << i;
    }
    storage << "]";
}
