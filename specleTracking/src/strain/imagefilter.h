#ifndef IMAGEFILTER_H
#define IMAGEFILTER_H

#include <QObject>

#include <opencv2/opencv.hpp>

#include "linalg/common.h"

class ImageFilterBase : public QObject
{
    Q_OBJECT

public:
    ImageFilterBase(QObject *parent = 0) : QObject(parent) { }

    virtual void process(Mat8 &frame) = 0;

    VectorOfImages calculateMean(VectorOfImages &inputImages, int windowSize);
};

typedef std::list<ImageFilterBase*> ListOfImageProcessing;


class ImageFilterMedian : public ImageFilterBase
{
    Q_OBJECT

public:
    int ksize;

    ImageFilterMedian(int ksize = 7, QObject *parent = 0) : ImageFilterBase(parent), ksize(ksize) { }

    virtual void process(Mat8 &frame)
    {
        cv::medianBlur(frame, frame, ksize);
    }
};

class ImageFilterHistEq : public ImageFilterBase
{
    Q_OBJECT

public:
    ImageFilterHistEq(QObject *parent = 0) : ImageFilterBase(parent) { }

    virtual void process(Mat8 &frame)
    {
        cv::equalizeHist(frame, frame);
    }
};

class ImageFilterNlMeansDenoise : public ImageFilterBase
{
    Q_OBJECT

public:
    float h;
    int searchSize;
    int templateSize;

    ImageFilterNlMeansDenoise(float regulation = 3, int searchWindowSize = 21, int templateWindowSize = 7, QObject *parent = 0) :
        ImageFilterBase(parent), h(regulation), searchSize(searchWindowSize), templateSize(templateWindowSize) {}

    virtual void process(Mat8 &frame)
    {
        Mat8 result;
        cv::fastNlMeansDenoising(frame, result, h, templateSize, searchSize);
        result.copyTo(frame);
    }

private:
    float weight(Mat8 &frame, P reference, P target);
};

class ImageFilterContrast : public ImageFilterBase
{
    Q_OBJECT

public:
    float alfa;
    float beta;

    ImageFilterContrast(float alfa = 1.0f, float beta = 0.0f, QObject *parent = 0) :
        ImageFilterBase(parent), alfa(alfa), beta(beta) {}

    virtual void process(Mat8 &frame)
    {
        frame.convertTo(frame, -1, alfa, beta);
    }
};

class ImageFilterEdge : public ImageFilterBase
{
    Q_OBJECT

public:
    ImageFilterEdge(QObject *parent = 0) : ImageFilterBase(parent) { }

    virtual void process(Mat8 &frame)
    {
        cv::Mat dest(frame.rows, frame.cols, CV_16S);
        cv::Sobel(frame, dest, CV_16S, 1, 1);
        dest.convertTo(frame, CV_8UC1, 0.5, 127);
    }
};

#endif // IMAGEFILTER_H
