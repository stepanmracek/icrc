#include "anotation.h"

#include "anotationdata.h"
#include "linalg/spline.h"

void onAnotationMouse(int event, int x, int y, int /*flags*/, void* param)
{
    AnotationData *data = (AnotationData*)param;
    Spline spline;

    if (event == CV_EVENT_LBUTTONUP)
    {
        P p(x,y);
        data->points.push_back(p);
        Mat8 imageWithSpline = data->image.clone();

        spline.drawSpline(data->points, imageWithSpline, false);
        cv::imshow("anotation", imageWithSpline);

        //std::cout << "new point " << p.x << " " << p.y << "; total: " << data->points.size() << std::endl;
    }
    else if (event == CV_EVENT_RBUTTONUP)
    {
        if (data->points.size() > 0)
        {
            data->points.pop_back();
            Mat8 imageWithSpline = data->image.clone();
            spline.drawSpline(data->points, imageWithSpline, false);
            cv::imshow("anotation", imageWithSpline);
        }
    }
}

VectorOfShapes Anotation::anotateFromVideo(VideoDataBase &video)
{
    VectorOfShapes shapes;
    Mat8 frame;
    video.setIndex(0);
    video.getNextFrame(frame);
    Spline spline;

    AnotationData anotationData;
    anotationData.image = frame;

    cv::namedWindow("anotation");
    cv::setMouseCallback("anotation", onAnotationMouse, &anotationData);
    cv::imshow("anotation", frame);

    while (1)
    {
        char key = cv::waitKey(0);
        if (key == 27) break;
        if (key == ' ')
        {
            if (anotationData.points.size() > 2)
            {
                //Points uniformPoints = spline.uniformDistance(anotationData.points, 15, false);
                //Mat8 frameWithPoints = frame.clone();
                //spline.drawSpline(anotation, frameWithPoints, false);

                Points copy(anotationData.points);
                shapes.push_back(copy);

                //cv::imshow("anotation", frameWithPoints);
                //cv::waitKey(1000);

                std::cout << "stored " << shapes.size() << " shapes." << std::endl;
            }
            anotationData.points.clear();

            bool readResult = video.getNextFrame(frame);
            if (!readResult) break;
            anotationData.image = frame;
            cv::imshow("anotation", frame);
        }
    }

    return shapes;
}

Points Anotation::anotateFromFrame(Mat8 &frame)
{
    AnotationData anotationData;
    anotationData.image = frame;

    // manual anotation of data in initial frame
    cv::namedWindow("anotation");
    cv::setMouseCallback("anotation", onAnotationMouse, &anotationData);
    cv::imshow("anotation", frame);
    cv::waitKey(0);

    return anotationData.points;
}

void Anotation::showShape(Mat8 &frame, Points &shape)
{
    Mat8 clone = frame.clone();
    Spline spline;
    spline.drawSpline(shape, clone, false, 255);
    cv::imshow("shape", clone);
    cv::waitKey();
}
