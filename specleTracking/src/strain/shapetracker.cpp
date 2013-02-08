#include "shapetracker.h"

#include <iostream>

#include "anotationdata.h"
#include "statisticalshapemodel.h"
#include "coordsystem.h"
#include "linalg/spline.h"

void drawAllPoints(Mat8 &image, Points &points)
{
    //int n = points.size();
    //if (n == 0) return;
    //for (int i = 0; i < n; i++)
    //    cv::circle(image, points.at(i), 3, cv::Scalar(255));
    Spline spline;
    spline.drawSpline(points, image, true);
}

void applyProcessing(Mat8 &frame, ListOfImageProcessing &filters)
{
    for (ListOfImageProcessing::iterator it = filters.begin(); it != filters.end(); ++it)
    {
        ImageFilterBase *filter = *it;
        filter->process(frame);
    }
}

Points ShapeTracker::track(VectorOfImages &prevFrames, VectorOfShapes &prevShapes, Mat8 &nextFrame)
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
        applyProcessing(transformedPrevFrame, frameProcessing);
        transformedPrevFrames.push_back(transformedPrevFrame);
    }
    Mat8 transformedNextFrame = coordSystem->transform(nextFrame);
    applyProcessing(transformedNextFrame, frameProcessing);

    // track
    Points transformedNextPoints;
    pointTracker.track(transformedPrevFrames, weights, transformedNextFrame,
                       transformedPrevShapes, transformedNextPoints);

    // back-transform to original space
    Points nextPointsInOriginalSpace = coordSystem->backTransformPoints(transformedNextPoints);

    // normalize points
    Points normalizedPoints = strain.shapeNormalizer.normalize(nextPointsInOriginalSpace, nextFrame);

    return normalizedPoints;
}

//Points ShapeTracker::track(Mat8 &prevFrame, Points &prevPoints, Mat8 &nextFrame, Strain &strain, CoordSystemBase *coordSystem, ListOfImageProcessing &frameProcessing, PointTrackerBase &pointTracker)
//{
//
//}

/*VectorOfShapes ShapeTracker::track(VideoDataBase &data, Strain &strain, ListOfProcessing &frameProcessing,
                                   PointTrackerBase &pointTracker, StrainResultProcessingBase &resultProcessing,
                                   Points &initialPoints, bool showVideo)
{
    VectorOfShapes result;

    data.setIndex(0);
    Mat8 firstframe;
    data.getNextFrame(firstframe);

    int pointCount = strain.shapeNormalizer.getNumberOfPoints();

    // make uniform distance between initial points
    //Spline spline;
    Points uniformPointsInOriginalCoorSystem = spline.uniformDistance(initialPoints, pointCount, true);
    result.push_back(uniformPointsInOriginalCoorSystem);

    // transform point to desired coord system
    Points uniformPoints = strain.coordSystem.transformPoints(uniformPointsInOriginalCoorSystem);

    if (showVideo)
    {
        Mat8 firstFrameWithUniformPoints = strain.coordSystem.transform(firstframe);
        drawAllPoints(firstFrameWithUniformPoints, uniformPoints);

        cv::imshow("transformed anotation", firstFrameWithUniformPoints);
        cv::waitKey(0);
    }

    // create video window
    const char *winName = "video";
    if (showVideo)
        cv::namedWindow(winName);
    Mat8 prevFrame = firstframe.clone();
    Mat8 transformedPrevFrame = strain.coordSystem.transform(prevFrame);
    applyProcessing(transformedPrevFrame, frameProcessing);
    Mat8 nextFrame, transformedNextFrame;

    // initiate point tracking
    Points prevPoints = uniformPoints;
    Points nextPoints = uniformPoints;
    Points nextPointsInOriginalSpace;
    int counter = 1;
    while (data.getNextFrame(nextFrame))
    {
        std::cout << "frame: " << (counter++) << std::endl;

        // transform and aplly processing filters
        transformedNextFrame = strain.coordSystem.transform(nextFrame);
        applyProcessing(transformedNextFrame, frameProcessing);

        // track
        pointTracker.track(transformedPrevFrame, transformedNextFrame, prevPoints, nextPoints);

        // normalize points
        nextPoints = strain.shapeNormalizer.normalize(nextPoints, transformedNextFrame);

        // append to result
        nextPointsInOriginalSpace = strain.coordSystem.backTransformPoints(nextPoints);

        // draw?
        if (showVideo)
        {
            drawAllPoints(transformedNextFrame, nextPoints);
            cv::imshow(winName, transformedNextFrame);

            drawAllPoints(nextFrame, nextPointsInOriginalSpace);
            cv::imshow("original", nextFrame);

            if (cv::waitKey(50) >= 0) break;
        }

        // copy current frame as previous one for the next iteration
        prevFrame = nextFrame.clone();
        transformedPrevFrame = transformedNextFrame.clone();
        prevPoints = nextPoints;
    }

    VectorOfShapes processedResult = resultProcessing.process(result, data);
    return processedResult;
}*/

