/*
 * spline.cpp
 *
 *  Created on: 8.6.2012
 *      Author: stepo
 */

#include "spline.h"

#include <iostream>
#include <cmath>

Spline::Spline()
{
    splineMatrix = (MatF(4,4) <<
                     0,  2,  0,  0,
                    -1,  0,  1,  0,
                     2, -5,  4, -1,
                    -1,  3, -3,  1);
}

void Spline::drawSpline(Points &controllPoints, Mat8 &image, bool circle, unsigned char intensity)
{
    Points splinePoints = circle ?
                getCircleSplinePoints(controllPoints, 20)
              : getSplinePoints(controllPoints, 20);

	int n = splinePoints.size();
	for (int i = 0; i < n; i++)
    {
        int x = splinePoints[i].x;
        int y = splinePoints[i].y;
        if (x >= 0 && x < image.cols && y >= 0 && y < image.rows)
            image(y, x) = intensity;
    }

    n = controllPoints.size();
	for (int i = 0; i < n; i++)
	{
        cv::circle(image, controllPoints[i], i == 0 ? 5 : 3, cv::Scalar(intensity));
	}        
}

Points Spline::getCircleSplinePoints(Points &points, int pointsPerSegment)
{
    Points out;
    int n = points.size();
    if (n == 2)
    {
        Points initSegment = getSplinePoints(points[0], points[0], points[1], points[1], pointsPerSegment);
        int segLen = initSegment.size();
        for (int j = 0; j < segLen; j++)
            out.push_back(initSegment[j]);
    }
    else if (n >= 3)
    {
        for (int i = 0; i < n; i++)
        {
            Points segment = getSplinePoints(points[i], points[(i+1)%n], points[(i+2)%n], points[(i+3)%n], pointsPerSegment);
            int segLen = segment.size();
            for (int j = 0; j < segLen; j++)
                out.push_back(segment[j]);
        }
    }

    return out;
}

Points Spline::getSplinePoints(Points &points, int pointsPerSegment)
{
	Points out;
	int n = points.size();
	int startIndex = 1; // 0 if curve starts at first point
	int endIndex = n - 3; // n-2 if curve ends at last point
	if (n == 2)
	{
		Points initSegment = getSplinePoints(points[0], points[0], points[1], points[1], pointsPerSegment);
		int segLen = initSegment.size();
		for (int j = 0; j < segLen; j++)
			out.push_back(initSegment[j]);
	}
	if (n >= 3)
	{
		Points initSegment = getSplinePoints(points[0], points[0], points[1], points[2], pointsPerSegment);
		int segLen = initSegment.size();
		for (int j = 0; j < segLen; j++)
			out.push_back(initSegment[j]);
	}
	for (int i = startIndex; i <= endIndex; i++)
	{
		Points segment = getSplinePoints(points[i-1], points[i], points[i+1], points[i+2], pointsPerSegment);
		int segLen = segment.size();
		for (int j = 0; j < segLen; j++)
			out.push_back(segment[j]);
	}
	if (n >= 3)
	{
		Points lastSegment = getSplinePoints(points[n-3], points[n-2], points[n-1], points[n-1], pointsPerSegment);
		int segLen = lastSegment.size();
		for (int j = 0; j < segLen; j++)
			out.push_back(lastSegment[j]);
	}
    out.push_back(points[n-1]);
	return out;
}

Points Spline::getSplinePoints(P &p1, P &p2, P &p3, P &p4, int pointsPerSegment)
{
	Points out;
	MatF Pmat = (MatF(4, 2) <<
			p1.x, p1.y,
			p2.x, p2.y,
			p3.x, p3.y,
			p4.x, p4.y);
	MatF MP = splineMatrix*Pmat;

	double dt = 1.0f/pointsPerSegment;
    for (float t = 0; t < 1; t += dt)
	{
		float t2 = t*t;
		float t3 = t2*t;
		MatF T = (MatF(1, 4) << 1, t, t2, t3);
		MatF Q = 0.5f*T*MP;

		P p(Q(0), Q(1));
		out.push_back(p);
	}
	return out;
}

Points Spline::uniformDistance(Points &in, int segments, bool circle)
{
	Points out;
    Points allPointsOfSpline = circle ?
                getCircleSplinePoints(in)
              : getSplinePoints(in);

    if (circle)
    {
        // circle -> topmost point is a starting one
        allPointsOfSpline = reorganizePoints(allPointsOfSpline);
    }

    // calculate total length of a curve
    int n = allPointsOfSpline.size();
    float len = length(allPointsOfSpline);
	float segmentLen = len/segments;
    int segmentCounter = 1;
	out.push_back(allPointsOfSpline[0]);
	float cumulativeLen = 0.0;
	for (int i = 1; i < n; i++)
	{
		// cumulative length of curve
        float d = Common::eucl(allPointsOfSpline[i-1], allPointsOfSpline[i]);
		cumulativeLen += d;

		// new segment needed
        if (cumulativeLen >= segmentCounter*segmentLen && segmentCounter < segments)
		{
            out.push_back(allPointsOfSpline[i-1]);
			segmentCounter++;
		}
	}

	// add last point
    if (!circle && (int)out.size() <= segments)
	{
		out.push_back(allPointsOfSpline[n-1]);
	}

	return out;
}

Points Spline::reorganizePoints(Points &input)
{
    int n = input.size();
    int minIndex = -1;
    float min = 1e300;
    for (int i = 0; i < n; i++)
    {
        float v = input[i].y*input[i].y + input[i].x*input[i].x;
        if (v < min)
        {
            min = v;
            minIndex = i;
        }
    }
    assert(minIndex != -1);

    Points result;
    for (int i = 0; i < n; i++)
        result.push_back(input[(i+minIndex)%n]);
    return result;
}

float Spline::length(Points &splinePoints)
{
    float len = 0;
    int n = splinePoints.size();
    for (int i = 1; i < n; i++)
    {
        len += Common::eucl(splinePoints[i-1], splinePoints[i]);
    }
    return len;
}

P Spline::getPoint(Points &splinePoints, float t)
{
    int n = splinePoints.size();
    if (n == 0) return P();
    if (t < 0) return splinePoints[0];
    if (t > 1) return splinePoints[n-1];

    float totalLen = length(splinePoints);
    float cumulativeLen = 0;
    int nearestIndex = -1;
    float nearestDelta = 1.1; // anything greater than 1
    for (int i = 0; i < n; i++)
    {
        cumulativeLen += Common::eucl(splinePoints[0], splinePoints[i]);
        float currentT = cumulativeLen/totalLen;
        float currentDelta = fabs(currentT - t);
        if (currentDelta < nearestDelta)
        {
            nearestIndex = i;
            nearestDelta = currentDelta;
        }
    }

    if (nearestIndex == -1) return P();
    else return splinePoints[nearestIndex];
}
