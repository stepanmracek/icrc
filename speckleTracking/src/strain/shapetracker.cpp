#include "shapetracker.h"

#include <QList>
#include <iostream>

#include "anotationdata.h"
#include "statisticalshapemodel.h"
#include "coordsystem.h"
#include "linalg/spline.h"

void drawAllPoints(Mat8 &image, Points &points)
{
    Spline spline;
    spline.drawSpline(points, image, true);
}

void ShapeTracker::applyProcessing(Mat8 &frame)
{
    foreach (ImageFilterBase *filter, frameProcessing)
    {
        filter->process(frame);
    }
}

QString ShapeTracker::getInfo()
{
    QString text = strain->getInfo() + pointTracker->getInfo() + resultProcessing->getInfo() + "Image processing:\n";

    if (frameProcessing.count() == 0)
    {
        text += "    (none)\n";
    }
    else
    {
        foreach (ImageFilterBase *filter, frameProcessing)
        {
            text += QString("    ") + filter->getInfo() + "\n";
        }
    }

    text += "Weights:\n    ";
    foreach(const float &w, this->weights)
    {
        text += QString::number(w) + ", ";
    }
    text += "\n";

    return text;
}

Points ShapeTracker::track(VectorOfImages &prevFrames, VectorOfShapes &prevShapes, Mat8 &nextFrame, CoordSystemBase *coordSystem)
{
    unsigned int n = prevFrames.size();
    assert(n == prevShapes.size());
    assert(n <= weights.size());

    // transform input shapes according to the given CoordSystem
    VectorOfShapes transformedPrevShapes = coordSystem->transformShapes(prevShapes);

    // image processing
    VectorOfImages transformedPrevFrames;
    for (unsigned int i = 0; i < n; i++)
    {
        Mat8 transformedPrevFrame = coordSystem->transform(prevFrames[i]);
        applyProcessing(transformedPrevFrame);
        transformedPrevFrames.push_back(transformedPrevFrame);
    }
    Mat8 transformedNextFrame = coordSystem->transform(nextFrame);
    applyProcessing(transformedNextFrame);

    // track
    Points transformedNextPoints;
    pointTracker->track(transformedPrevFrames, weights, transformedNextFrame,
                       transformedPrevShapes, transformedNextPoints);

    // back-transform to original space
    Points nextPointsInOriginalSpace = coordSystem->backTransformPoints(transformedNextPoints);

    // normalize points
    Points normalizedPoints = strain->getShapeNormalizer()->normalize(nextPointsInOriginalSpace, nextFrame);

    return normalizedPoints;
}

ShapeMap ShapeTracker::track(const VideoDataClip *clip, int startIndex, int endIndex, const Points &initialShape, QProgressDialog *progress)
{
    ShapeMap resultShapes;
    resultShapes[startIndex] = initialShape;

    int frames = endIndex - startIndex - 1;
    for (int i = 0; i < frames; i++)
    {
        if (progress != 0)
        {
            if (progress->wasCanceled())
            {
                break;
            }

            progress->setValue(i);
        }

        // prepare previous shapes and previous frames
        VectorOfShapes prevShapes;
        VectorOfImages prevFrames;

        for (unsigned int j = 0; j < weights.size(); j++)
        {
            int prevIndex = startIndex + i-j;

            if (prevIndex < startIndex)
            {
                break;
            }

            prevShapes.push_back(resultShapes[prevIndex]);
            prevFrames.push_back(clip->frames[prevIndex]);
        }

        int nextIndex = startIndex + i + 1;
        Mat8 nextFrame = clip->frames[nextIndex];

        std::reverse(prevShapes.begin(), prevShapes.end());
        std::reverse(prevFrames.begin(), prevFrames.end());

        // track
        Points nextShape = track(prevFrames, prevShapes, nextFrame, clip->getMetadata()->getCoordSystem());
        resultShapes[nextIndex] = nextShape;
    }

    if (progress != 0)
    {
        progress->setValue(frames);
    }

    return resultProcessing->process(resultShapes, startIndex, endIndex, clip);
}

ShapeTracker *ShapeTracker::getDummyTracker()
{
    ShapeNormalizerPass *normalizer = new ShapeNormalizerPass();
    LongitudinalStrain *strain = new LongitudinalStrain(normalizer, 6, 5);
    ListOfImageProcessing listOfProcessing;
    PointTrackerOpticalFlow *pointTracker = new PointTrackerOpticalFlow(20);
    StrainResultProcessingPass *resultProcessing = new StrainResultProcessingPass();
    VectorF weights; weights.push_back(1.0);
    return new ShapeTracker(strain, listOfProcessing, pointTracker, resultProcessing, weights);
}

void ShapeTracker::clearFrameProcessing()
{
    qDeleteAll(frameProcessing);
    frameProcessing.clear();
}

void ShapeTracker::addFilters(QList<ImageFilterBase *> newFilters)
{
    foreach(ImageFilterBase *f, newFilters)
    {
        frameProcessing.append(f);
        f->setParent(this);
    }
}
