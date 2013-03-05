#ifndef IMAGEFILTER_H
#define IMAGEFILTER_H

#include <QObject>
#include <QList>

#include <opencv2/opencv.hpp>

#include "linalg/common.h"

/**
 * @brief Base class of all image filters
 */
class ImageFilterBase : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Constructor
     * @param parent Parent object in Qt hierarchy
     */
    ImageFilterBase(QObject *parent = 0) : QObject(parent) { }

    /**
     * @brief Applies the image filter to desired image
     * @param frame Image
     */
    virtual void process(Mat8 &frame) = 0;

    /**
     * @brief Process image filter on vector of images.
     * @param inputImages Input images
     * @param windowSize Desired window size. Filter on all images within the window is calculated and the mean is added to the result
     * @return Vector of resulting means
     */
    VectorOfImages calculateMean(VectorOfImages &inputImages, int windowSize);
};

/**
 * @brief Typedef for List of image filters
 */
typedef QList<ImageFilterBase*> ListOfImageProcessing;


/**
 * @brief Median image filter
 */
class ImageFilterMedian : public ImageFilterBase
{
    Q_OBJECT

private:
    int ksize;

public:

    /**
     * @brief Constructor
     * @param ksize Kernel size
     * @param parent Parent object in Qt hierarchy
     */
    ImageFilterMedian(int ksize = 7, QObject *parent = 0) : ImageFilterBase(parent), ksize(ksize) { }

    virtual void process(Mat8 &frame)
    {
        cv::medianBlur(frame, frame, ksize);
    }
};

/**
 * @brief Histogram equalization
 */
class ImageFilterHistEq : public ImageFilterBase
{
    Q_OBJECT

public:

    /**
     * @brief Constructor
     * @param parent Parent object in Qt hierarchy
     */
    ImageFilterHistEq(QObject *parent = 0) : ImageFilterBase(parent) { }

    virtual void process(Mat8 &frame)
    {
        cv::equalizeHist(frame, frame);
    }
};

/**
 * @brief The Non-linear means denoise filter
 */
class ImageFilterNlMeansDenoise : public ImageFilterBase
{
    Q_OBJECT

private:
    float h;
    int searchSize;
    int templateSize;

public:

    /**
     * @brief Constructor
     * @param regulation Regulation coefficient
     * @param searchWindowSize Search window size
     * @param templateWindowSize Template Window Size
     * @param parent Parent object in Qt hierarchy
     */
    ImageFilterNlMeansDenoise(float regulation = 3, int searchWindowSize = 21, int templateWindowSize = 7, QObject *parent = 0) :
        ImageFilterBase(parent), h(regulation), searchSize(searchWindowSize), templateSize(templateWindowSize) {}

    virtual void process(Mat8 &frame)
    {
        Mat8 result;
        cv::fastNlMeansDenoising(frame, result, h, templateSize, searchSize);
        result.copyTo(frame);
    }

/*private:
    float weight(Mat8 &frame, P reference, P target);*/
};

/**
 * @brief Contrast filter
 */
class ImageFilterContrast : public ImageFilterBase
{
    Q_OBJECT

private:
    float alfa;
    float beta;

public:

    /**
     * @brief Constructor
     * @param alfa Coefficient alfa
     * @param beta Coefficient beta
     * @param parent Parent object in Qt hierarchy
     */
    ImageFilterContrast(float alfa = 1.0f, float beta = 0.0f, QObject *parent = 0) :
        ImageFilterBase(parent), alfa(alfa), beta(beta) {}

    virtual void process(Mat8 &frame)
    {
        frame.convertTo(frame, -1, alfa, beta);
    }
};

/**
 * @brief Edge detection filter
 */
class ImageFilterEdge : public ImageFilterBase
{
    Q_OBJECT

private:
    int ksize;

public:
    /**
     * @brief Constructor
     * @param ksize Kernel size
     * @param parent Parent object in Qt hierarchy
     */
    ImageFilterEdge(int ksize, QObject *parent = 0) : ksize(ksize), ImageFilterBase(parent) { }

    virtual void process(Mat8 &frame)
    {
        cv::Mat dest(frame.rows, frame.cols, CV_16S);
        cv::Sobel(frame, dest, CV_16S, 1, 1, ksize);
        dest.convertTo(frame, CV_8UC1, 0.5, 127);
    }
};

#endif // IMAGEFILTER_H
