#ifndef SHAPECOMPARER_H
#define SHAPECOMPARER_H

#include "common.h"

class ShapeComparer
{
public:
    /**
     * @brief Direct comparison of two shapes point by point
     * @param first First shape
     * @param second Seconf shape
     * @return Returns mean sum of square differences
     */
    static float directDiff(const Points &first, const Points &second);

    /**
     * @brief Align (translation and rotation) and Compares two shapes
     * @param first First shape
     * @param second Seconf shape
     * @return Returns mean sum of square differences
     */
    static float optimalDiff(const Points &first, const Points &second);
};

#endif // SHAPECOMPARER_H
