#ifndef SPLINE_H_
#define SPLINE_H_

#include "common.h"

/**
 * @brief Spline that connects all constrol points
 */
class Spline
{
public:
    /**
     * @brief Spline constructor
     */
	Spline();

    /**
     * @brief Draws spline between all control points
     * @param controlPoints Control points
     * @param image Target image
     * @param circle Draw circle in circle?
     * @param intensity Intensity of drawn spline
     */
    void drawSpline(const Points &controlPoints, Mat8 &image, bool circle, unsigned char intensity = 255);

    /**
     * @brief Returns all points among spline control points in circle
     * @param points Control points
     * @param pointsPerSegment Number of point per each segment
     * @return All points among spline control points in circle
     */
    Points getCircleSplinePoints(const Points &points, int pointsPerSegment = 20);

    /**
     * @brief Returns all points among spline control points
     * @param points Control points
     * @param pointsPerSegment Number of point per each segment
     * @return All points among spline control points
     */
    Points getSplinePoints(const Points &points, int pointsPerSegment = 20);

    /**
     * @brief Returns specific number of points among spline with uniform distance
     * @param controlPoints Input control points of the spline
     * @param count Count of the new spline points
     * @return Points among spline with uniform distance
     */
    Points uniformDistance(const Points &controlPoints, int count);

private:
    P getPoint(Points &splinePoints, float t);

    Points reorganizePoints(Points &input);

    Points getSplinePoints(const P &p1, const P &p2, const P &p3, const P &p4, int pointsPerSegment = 20);

    float length(Points &splinePoints);

    MatF splineMatrix;
};

#endif /* SPLINE_H_ */
