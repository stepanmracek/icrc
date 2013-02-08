/*
 * spline.h
 *
 *  Created on: 8.6.2012
 *      Author: stepo
 */

#ifndef SPLINE_H_
#define SPLINE_H_

#include "common.h"

class Spline
{
public:
	Spline();
	virtual ~Spline() {}

    void drawSpline(Points &controllPoints, Mat8 &image, bool circle, unsigned char intensity = 255);
	//void drawSpline(P &p1, P &p2, P &p3, P &p4, Mat8 &image, unsigned char intensity = 255);

    Points getCircleSplinePoints(Points &points, int pointsPerSegment = 20);
	Points getSplinePoints(Points &points, int pointsPerSegment = 20);
	Points getSplinePoints(P &p1, P &p2, P &p3, P &p4, int pointsPerSegment = 20);

    Points uniformDistance(Points &in, int segments, bool circle);

    P getPoint(Points &splinePoints, float t);

    float length(Points &splinePoints);

	MatF splineMatrix;

private:
    Points reorganizePoints(Points &input);
};

#endif /* SPLINE_H_ */
