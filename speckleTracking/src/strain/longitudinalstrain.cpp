#include "longitudinalstrain.h"

#include "shapeprocessing.h"
#include <QDebug>

LongitudinalStrain::LongitudinalStrain(ShapeNormalizerBase *shapeNormalizer, QObject *parent) :
    Strain(shapeNormalizer, 6, 5, parent)
{
    //segmentsCount = 6;
    //pointsPerSegment = 5;
}

Points LongitudinalStrain::getRealShapePoints(const Points &controlPoints, int shapeWidth)
{
    Points result;
    int n = controlPoints.size();
    if (n <= 1) return result;

    Points uniformMidControlPoints = spline.uniformDistance(controlPoints, segmentsCount, false);
    Points uniformMidPoints = spline.getSplinePoints(uniformMidControlPoints, pointsPerSegment);
    int uniformSize = uniformMidPoints.size();

    Points uniformInnerPoints;
    Points uniformOuterPoints;
    for (int i = 0; i < uniformSize; i++)
    {
        int prevIndex, nextIndex;
        if (i == 0)
        {
            prevIndex = 0;
            nextIndex = 1;
        }
        else if (i == uniformSize-1)
        {
            prevIndex = uniformSize-2;
            nextIndex = uniformSize-1;
        }
        else
        {
            prevIndex = i-1;
            nextIndex = i+1;
        }

        P &prev = uniformMidPoints[prevIndex];
        P &cur = uniformMidPoints[i];
        P &next = uniformMidPoints[nextIndex];

        float dx = next.x - prev.x;
        float dy = next.y - prev.y;
        float dMag = sqrt(dx*dx + dy*dy);

        float midx = cur.x - dy/dMag*shapeWidth/2.0;
        float midy = cur.y + dx/dMag*shapeWidth/2.0;
        uniformInnerPoints.push_back(P(midx, midy));

        float outx = cur.x + dy/dMag*shapeWidth/2.0;
        float outy = cur.y - dx/dMag*shapeWidth/2.0;
        uniformOuterPoints.push_back(P(outx, outy));
    }

    for (int i = 0; i < uniformSize; i++)
    {
        result.push_back(uniformInnerPoints[i]);
        result.push_back(uniformMidPoints[i]);
        result.push_back(uniformOuterPoints[i]);
    }

    Mat8 m;
    return getShapeNormalizer()->normalize(result, m);
}

/*Points LongitudinalStrain::getRealShapePoints(Points &controlPoints)
{
    Points result;
    int n = controlPoints.size();
    if (n <= 2) return result;

    Points uniformInnerControlPoints = spline.uniformDistance(controlPoints, 6, false);
    qDebug() << "uniformInnerControlPoints:" << uniformInnerControlPoints.size();

    Points uniformInnerPoints = spline.getSplinePoints(uniformInnerControlPoints, 5);
    qDebug() << "uniformInnerPoints:" << uniformInnerPoints.size();
    for (unsigned int i = 0; i < uniformInnerPoints.size(); i++)
    {
        qDebug() << "  " << i << uniformInnerPoints[i].x << uniformInnerPoints[i].y;
    }

    P base = P((controlPoints[0].x + controlPoints[n-1].x) / 2, (controlPoints[0].y + controlPoints[n-1].y) / 2);

    Points uniformMidPoints = ShapeProcessing::outline(uniformInnerPoints, shapeWidth/2, base);
    Points uniformOuterPoints = ShapeProcessing::outline(uniformInnerPoints, shapeWidth, base);

    int uniformSize = uniformInnerPoints.size();
    for (int i = 0; i < uniformSize; i++)
    {
        result.push_back(uniformInnerPoints[i]);
        result.push_back(uniformMidPoints[i]);
        result.push_back(uniformOuterPoints[i]);
    }

    return result;
}*/

QList<QGraphicsItem*> LongitudinalStrain::drawResult(QGraphicsScene *scene, Points &resultPoints)
{
    QList<QGraphicsItem*> newItems;

    int n = resultPoints.size();
    if (n < 3) return newItems;

    QPen linePen(QColor(255, 255, 255, 127));
    for (int i = 3; i < n; i += 3)
    {
        P &p = resultPoints[i-2];
        P &n = resultPoints[i+1];
        QGraphicsItem *item = scene->addLine(p.x, p.y, n.x, n.y, linePen);
        newItems << item;
    }

    for (int i = 0; i < n; i += 3*this->pointsPerSegment)
    {
        P &in = resultPoints[i];
        P &mid = resultPoints[i+1];
        P &out = resultPoints[i+2];
        QGraphicsItem *item1 = scene->addLine(in.x, in.y, mid.x, mid.y, linePen);
        QGraphicsItem *item2 = scene->addLine(mid.x, mid.y, out.x, out.y, linePen);
        newItems << item1;
        newItems << item2;
    }

    P apex = getApexPoint(resultPoints);
    P base = getBasePoint(resultPoints);
    QGraphicsItem *baseLine = scene->addLine(base.x, base.y, apex.x, apex.y, linePen);
    newItems << baseLine;

    QPen pen;
    QBrush brush(Qt::SolidPattern);
    for (int i = 0; i < n; i++)
    {
        double hue = (double)i/n;
        pen.setColor(QColor::fromHsvF(hue, 1, 1));
        brush.setColor(QColor::fromHsvF(hue, 1, 1, 0.5));

        P &p = resultPoints[i];
        QGraphicsItem *item = scene->addEllipse(p.x-2, p.y-2, 4, 4, pen, brush);
        newItems << item;
    }

    return newItems;
}

P LongitudinalStrain::getBasePoint(Points &realPoints)
{
    int n = realPoints.size();

    P& first = realPoints[1];
    P& last = realPoints[n-2];

    return P((first.x+last.x)/2, (first.y+last.y)/2);
}

float len(const P& p)
{
    return sqrt(p.x*p.x + p.y*p.y);
}

P LongitudinalStrain::getApexPoint(Points &realPoints)
{
    //int n = realPoints.size();
    //return realPoints[n/2];

    int count = realPoints.size();
    assert(count > 1);
    P a = realPoints[1];
    P b = realPoints[count-2];
    P n = (b-a);
    float ln = len(n);
    n.x = n.x / ln;
    n.y = n.y / ln;
    float maxLen = 0;
    int maxIndex = -1;
    for (int i = 1; i < count; i += 3)
    {
        const P &p = realPoints[i];
        float d = len ((a-p) - ((a-p).dot(n)*n));
        if (d > maxLen)
        {
            maxLen = d;
            maxIndex = i;
        }
    }

    if (maxIndex != -1)
    {
        return realPoints[maxIndex];
    }
    else
    {
        return realPoints[1];
    }
}

QString LongitudinalStrain::getInfo()
{
    return QString("Longitudinal strain\n    segments: %1, points per segment: %2\n").arg(segmentsCount).arg(pointsPerSegment)
            + getShapeNormalizer()->getInfo() + "\n";
}
