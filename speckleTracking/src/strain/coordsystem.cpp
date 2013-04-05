#include "coordsystem.h"

#include <opencv2/opencv.hpp>
#include <iostream>

P AngleDistanceToPoint(float angle, float distance)
{
    P p;
    p.y = sin(angle)*distance;
    p.x = cos(angle)*distance;
    return p;
}

CoordSystemRadial::CoordSystemRadial(QObject *parent) : CoordSystemBase(parent)
{
    init(P(263,0), P(632,258), P(10,360), 50, 200, 400);
}

CoordSystemRadial::CoordSystemRadial(P center, float startDistance, float endDistance, float angleStart,
                                     float angleEnd, int resultMatCols, int resultMatRows, QObject *parent) :
    CoordSystemBase(parent)
{
    init(center, startDistance, endDistance, angleStart, angleEnd, resultMatCols, resultMatRows);
}

CoordSystemRadial::CoordSystemRadial(P center, P arcStart, P arcEnd, float startDistance, int resultMatCols, int resultMatRows, QObject *parent) :
    CoordSystemBase(parent)
{
    init(center, arcStart, arcEnd, startDistance, resultMatCols, resultMatRows);
}

void CoordSystemRadial::init(CoordSystemRadial *src)
{
    init(src->center, src->startDistance, src->endDistance, src->angleStart, src->angleEnd, src->resultMatCols, src->resultMatRows);
}

void CoordSystemRadial::init(P center, float startDistance, float endDistance, float angleStart, float angleEnd, int resultMatCols, int resultMatRows)
{
    this->center = center;
    this->startDistance = startDistance;
    this->endDistance = endDistance;
    this->angleStart = angleStart;
    this->angleEnd = angleEnd;
    this->resultMatCols = resultMatCols;
    this->resultMatRows = resultMatRows;

    dAngle = angleEnd - angleStart;
    angleStep = dAngle / (resultMatCols-1);

    dDistance = endDistance - startDistance;
    distanceStep = dDistance / (resultMatRows-1);
}

void CoordSystemRadial::init(P center, P arcStart, P arcEnd, float startDistance, int resultMatCols, int resultMatRows)
{
    this->center = center;
    this->startDistance = startDistance;
    this->resultMatCols = resultMatCols;
    this->resultMatRows = resultMatRows;

    float toStart = Common::eucl(center, arcStart);
    float toEnd = Common::eucl(center, arcEnd);
    endDistance = (toStart+toEnd)/2;

    float dStartX = (arcStart.x - center.x) * endDistance / toStart;
    float dStartY = (arcStart.y - center.y) * endDistance / toStart;

    float dEndX = (arcEnd.x - center.x) * endDistance / toEnd;
    float dEndY = (arcEnd.y - center.y) * endDistance / toEnd;

    P start(center.x + dStartX, center.y + dStartY);
    P end(center.x + dEndX, center.y + dEndY);

    //float angleEnd;
    // arc start
    {
        float x = start.x - center.x;
        float y = start.y - center.y;
        angleStart = atan2(y, x);
        assert(angleStart > 0);
        //std::cout << angleStart << std::endl;
    }
    // arc end
    {
        float x = end.x - center.x;
        float y = end.y - center.y;
        angleEnd = atan2(y, x);
        assert(angleEnd > 0);
        //std::cout << angleEnd << std::endl;
    }
    assert(angleStart < angleEnd);
    assert(startDistance < endDistance);

    dAngle = angleEnd - angleStart;
    angleStep = dAngle / (resultMatCols-1);

    dDistance = endDistance - startDistance;
    distanceStep = dDistance / (resultMatRows-1);
}

Mat8 CoordSystemRadial::transform(const Mat8 &src)
{
    Mat8 result(resultMatRows, resultMatCols);
    for (int c = 0; c < resultMatCols; c++)
    {
        float angle = angleStart + c*angleStep;

        for (int r = 0; r < resultMatRows; r++)
        {
            float distance = r*distanceStep + startDistance;

            P p = AngleDistanceToPoint(angle, distance);
            //cv::circle(img, cv::Point(center.x+p.x, center.y+p.y), 1, 255);
            //std::cout << r << " " << c << " " << angle << " " << distance << " " << p.x << " " << p.y << std::endl ;
            result(r, resultMatCols - c - 1) = src(center.y+p.y, center.x+p.x);
        }
    }

    /*cv::imshow("image", img);
    cv::imshow("result", result);
    cv::waitKey();*/
    return result;
}

P CoordSystemRadial::transform(P input)
{
    float x = input.x - center.x;
    float y = input.y - center.y;
    float angle = atan2(y, x);
    float distance = Common::eucl(input, center);

    return P(resultMatCols - (angle-angleStart)/dAngle*resultMatCols - 1, (distance-startDistance)/dDistance*resultMatRows);
}

P CoordSystemRadial::transform(float inputX, float inputY)
{
    return transform(P(inputX, inputY));
}

P CoordSystemRadial::backTransform(P input)
{
    float angle = angleStart + (resultMatCols - input.x - 1)/resultMatCols * dAngle;
    float distance = startDistance + input.y/resultMatRows * dDistance;
    P p = AngleDistanceToPoint(angle, distance);
    p.x += center.x;
    p.y += center.y;
    return p;
}

P CoordSystemRadial::backTransform(float inputX, float inputY)
{
    return backTransform(P(inputX, inputY));
}

struct CSAnotationData
{
    Mat8 image;
    Points points;
};

void onCSMouse(int event, int x, int y, int /*flags*/, void* param)
{
    CSAnotationData *data = (CSAnotationData*)param;

    if (event == CV_EVENT_LBUTTONUP)
    {
        P p(x, y);
        std::cout << p.x << " " << p.y << std::endl << std::flush;
        data->points.push_back(p);
        cv::circle(data->image, p, 3, 255);
        cv::imshow("image", data->image);
    }
}

void CoordSystemRadial::AnotateAngleDistance(Mat8 &src, P &center, P &arcStart, P &arcEnd)
{
    cv::imshow("image", src);

    CSAnotationData anotationData;
    anotationData.image = src.clone();

    cv::setMouseCallback("image", onCSMouse, &anotationData);
    cv::waitKey(0);

    if (anotationData.points.size() < 3) return;

    center.x = anotationData.points[0].x;
    center.y = anotationData.points[0].y;

    arcStart.x = anotationData.points[1].x;
    arcStart.y = anotationData.points[1].y;

    arcEnd.x = anotationData.points[2].x;
    arcEnd.y = anotationData.points[2].y;
}

#include <QPainter>

QList<QGraphicsItem*> CoordSystemRadial::draw(QGraphicsScene *scene)
{
    QList<QGraphicsItem*> result;
    QPen pen(Qt::red);

    P p1 = AngleDistanceToPoint(angleStart, startDistance);
    P p2 = AngleDistanceToPoint(angleStart, endDistance);
    result << scene->addLine(center.x + p1.x, center.y + p1.y, center.x + p2.x, center.y + p2.y, pen);

    P p3 = AngleDistanceToPoint(angleEnd, startDistance);
    P p4 = AngleDistanceToPoint(angleEnd, endDistance);
    result << scene->addLine(center.x + p3.x, center.y + p3.y, center.x + p4.x, center.y + p4.y, pen);

    QPainterPath painterPath;
    painterPath.arcMoveTo(center.x - startDistance, center.y - startDistance,
                      2 * startDistance, 2 * startDistance,
                      - angleStart * 57.2957795);
    painterPath.arcTo(center.x - startDistance, center.y - startDistance,
                      2 * startDistance, 2 * startDistance,
                      - angleStart * 57.2957795, - (angleEnd - angleStart) * 57.2957795);
    result << scene->addPath(painterPath, pen);

    painterPath.arcMoveTo(center.x - endDistance, center.y - endDistance,
                      2 * endDistance, 2 * endDistance,
                      - angleStart * 57.2957795);
    painterPath.arcTo(center.x - endDistance, center.y - endDistance,
                      2 * endDistance, 2 * endDistance,
                      - angleStart * 57.2957795, - (angleEnd - angleStart) * 57.2957795);
    result << scene->addPath(painterPath, pen);

    return result;
}
